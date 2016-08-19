#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
//#include<time.h>
#include<sys/time.h>
#include<string>
#include<math.h>
#include<iomanip>
//#include<omp.h>
using namespace std;

struct velocity
{
    double x;
    double y;
    velocity(double x,double y):x(x),y(y){}
};

struct acceleration
{
    double x;
    double y;
    acceleration(double x,double y):x(x),y(y){}
};

struct body
{
    double mass;
    double x;
    double y;
    velocity *vel;
    acceleration *acc;

    body(double mass,double x,double y,velocity* vel,acceleration* acc):mass(mass),x(x),y(y),vel(vel),acc(acc){}
};

const int times=1;
//const double t=1;
const double dt=1;
const int num_bodies=100;
const double G=6.67384 * pow(10,-11);
//node *array[10000];
int index=1;
double force_x[100000]={0};
double force_y[100000]={0};

double distance(body *,body *);

int main(int argc,char **argv)
{
	velocity *vel[num_bodies+1];
    acceleration *acc[num_bodies+1];
    body *a[num_bodies+1];
    for(int i=1;i<=num_bodies;i++)
    {
        vel[i]=new velocity(0,0);
        acc[i]=new acceleration(0,0);
    }
    string line,line2;
    ifstream myfile2 ("../b100.txt");
    int i=1;
    if (myfile2.is_open())
    {
        while ( getline (myfile2,line) )
        {
            getline(myfile2,line2);
            a[i++]=new body(1e10,atoi(line.c_str()),atoi(line2.c_str()),vel[1],acc[1]);
        }
        myfile2.close();
    }
    else cout << "Unable to open file";
           MPI_Init(&argc, &argv); 
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    for(int o=1;o<=times;o++)
    {    
        if(rank==0)
        {
            //cout<<"Asd"<<endl;
            int stage1[num_bodies+1]={0};
            //cout<<"Master"<<endl;
            struct timeval t0,t1;
            gettimeofday(&t0,0);
            //cout<<"Asd"<<endl;
            //int flag2=1;
            //MPI_Send(&flag2,1,MPI_INT,0,0,MPI_COMM_WORLD);
            for(int p=1;p<=num_bodies;p++)
            {
                //cout<<"Received from "<<p<<endl;
                MPI_Recv(&stage1[p],1,MPI_INT,p,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                //cout<<"Received from "<<p<<" "<<flag[p]<<endl;
            }
            //cout<<"123"<<endl;
            int start=1;
            for(int p=1;p<=num_bodies;p++)
            {
                MPI_Send(&start,1,MPI_INT,p,0,MPI_COMM_WORLD);
            }
            for(int p=1;p<=num_bodies;p++)
            {
                //cout<<"Received from "<<p<<endl;
                MPI_Recv(&a[p]->x,2,MPI_DOUBLE,p,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                //cout<<"Received from "<<p<<" "<<flag[p]<<endl;
            }
            for(int u=1;u<=num_bodies;u++)
            {
                //cout<<setprecision(20)<<a[u]->x<<" "<<a[u]->y<<" "<<a[u]->vel->x<<" "<<a[u]->vel->y<<" "<<a[u]->acc->x<<" "<<a[u]->acc->y<<endl<<endl;
                cout<<setprecision(20)<<a[u]->x<<"      "<<a[u]->y<<" "<<endl;
            }
            //cout<<"Asd"<<endl;
            gettimeofday(&t1, 0);
            long long elapsed = (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;//std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
            cout<<"Time Taken in milliseconds= "<<elapsed<<endl;
        }
        else
        {
            //cout<<"Slave"<<" "<<rank<<" "<<a[rank]->x<<endl;
            
            double force2=0;
            //cout<<array[i]->com->cm<<" jakkas"<<endl;
            //force(array[i],ce,&force_x[i],&force_y[i]);
            //cout<<force_x[i]<<" "<<force_y[i]<<" ppppp"<<endl;
            for(int j=1;j<=num_bodies;j++)
            {

                double dist=distance(a[rank],a[j]);
                if(rank==j)
                    continue;
                else
                {
                    force2=(G*a[rank]->mass*a[j]->mass)/(dist*dist);
                    force_x[rank]=force_x[rank]+force2*(a[j]->x-a[rank]->x)/dist;
                    force_y[rank]=force_y[rank]+force2*(a[j]->y-a[rank]->y)/dist;
              
                }
                
            }
            int stage1=rank,start;
            MPI_Send(&stage1,1,MPI_INT,0,0,MPI_COMM_WORLD);
            MPI_Recv(&start,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            //cout<<"poi"<<endl;
            //MPI_Barrier(MPI_COMM_WORLD);
            //cout<<"Slave\n";
            //#pragma omp parallel for
            //fflush(stdout);
            
            double newx=0,newy=0,newax=0,neway=0,newvx=0,newvy=0;
            newax=force_x[rank]/a[rank]->mass;
            neway=force_y[rank]/a[rank]->mass;
            //cout<<" Rank "<<rank<<" "<<newax<<" "<<neway<<endl;
            a[rank]->acc->x+=newax;
            a[rank]->acc->y+=neway;
            a[rank]->vel->x+=(a[rank]->acc->x)*dt;
            a[rank]->vel->y+=(a[rank]->acc->y)*dt;
            a[rank]->x+=a[rank]->vel->x*dt;
            a[rank]->y+=a[rank]->vel->y*dt;
            //cout<<"Rank "<<a[rank]->x<<" "<<a[rank]->y<<endl;
            // int flag=1;
            
            MPI_Send(&a[rank]->x,2,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
            //cout<<setprecision(20)<<a[rank]->x<<" "<<a[rank]->y<<" "<<a[rank]->vel->x<<" "<<a[rank]->vel->y<<" "<<a[rank]->acc->x<<" "<<a[rank]->acc->y<<endl<<endl;
            
        }
    }

        MPI_Finalize();
    
}

double distance(body *b1,body *b2)
{
    return sqrt(pow(b1->x-b2->x,2)+pow(b1->y-b2->y,2));
}