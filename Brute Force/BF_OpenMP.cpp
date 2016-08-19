#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
//#include<time.h>
#include<sys/time.h>
#include<string>
#include<math.h>
#include<iomanip>
#include<omp.h>
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

int times=10;
//const double t=1;
const double dt=1;
int num_bodies=10;
const double G=6.67384 * pow(10,-11);
//node *array[10000];
int index=1;
double force_x[100000]={0};
double force_y[100000]={0};

double distance(body *,body *);

int main()
{
    string filename;
    int threads;
    cin>>times>>num_bodies>>filename>>threads;
	velocity *vel[num_bodies+1];
    acceleration *acc[num_bodies+1];
    body *a[num_bodies+1];
    for(int i=1;i<=num_bodies;i++)
    {
        vel[i]=new velocity(0,0);
        acc[i]=new acceleration(0,0);
    }
    string line,line2;
    ifstream myfile2 (filename.c_str());
    int i=1;
    if (myfile2.is_open())
    {
        while ( getline (myfile2,line) )
        {
            getline(myfile2,line2);
            a[i++]=new body(1,atoi(line.c_str()),atoi(line2.c_str()),vel[1],acc[1]);
        }
        myfile2.close();
    }
    else cout << "Unable to open file";
    struct timeval t0,t1;
    gettimeofday(&t0,0);
    for(int o=1;o<=times;o++)
    {    
        //compute(ce);
        //force_start(ce);
        omp_set_num_threads(threads);
        #pragma omp parallel for
        for(int i=1;i<=num_bodies;i++)
        {
        	double force2=0;
            //cout<<array[i]->com->cm<<" jakkas"<<endl;
            //force(array[i],ce,&force_x[i],&force_y[i]);
            //cout<<force_x[i]<<" "<<force_y[i]<<" ppppp"<<endl;
        	for(int j=1;j<=num_bodies;j++)
        	{

        		double dist=distance(a[i],a[j]);
        		if(i==j)
        			continue;
        		else
        		{
        			force2=(G*a[i]->mass*a[j]->mass)/(dist*dist);
        			force_x[i]=force_x[i]+force2*(a[j]->x-a[i]->x)/dist;
        			force_y[i]=force_y[i]+force2*(a[j]->y-a[i]->y)/dist;
        		}
        	}
        }
        cout<<endl<<endl<<endl;
        #pragma omp parallel for
        for(int i=1;i<=num_bodies;i++)
        {
            double newx=0,newy=0,newax=0,neway=0,newvx=0,newvy=0;
            newax=force_x[i]/a[i]->mass;
            neway=force_y[i]/a[i]->mass;
            a[i]->acc->x+=newax;
            a[i]->acc->y+=neway;
            a[i]->vel->x+=(a[i]->acc->x)*dt;
            a[i]->vel->y+=(a[i]->acc->y)*dt;
            a[i]->x+=a[i]->vel->x*dt;
            a[i]->y+=a[i]->vel->y*dt;
            cout<<setprecision(20)<<a[i]->x<<" "<<a[i]->y<<" "<<a[i]->vel->x<<" "<<a[i]->vel->y<<" "<<a[i]->acc->x<<" "<<a[i]->acc->y<<endl<<endl;
        }
    }
    gettimeofday(&t1, 0);
    long long elapsed = (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;//std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
    cout<<"Time Taken in milliseconds= "<<elapsed<<endl;
}

double distance(body *b1,body *b2)
{
    return sqrt(pow(b1->x-b2->x,2)+pow(b1->y-b2->y,2));
}