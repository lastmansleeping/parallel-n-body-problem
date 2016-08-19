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

struct COM
{
    double cm;
    double cx;
    double cy;
    COM(double cm,double cx,double cy):cm(cm),cx(cx),cy(cy){}
};

struct node
{
    double width;
    double posx;
    double posy;
    node *nw;
    node *ne;
    node *sw;
    node *se;
    COM *com;
    body *part;

    node(double width,double posx,double posy,node *nw=NULL,node *ne=NULL,node *sw=NULL,node *se=NULL,COM *com=NULL,body *part=NULL):width(width),posx(posx),posy(posy),nw(nw),ne(ne),sw(sw),se(se),com(com),part(part){}
};

int times=10;
string filename;
const double t=1;
const double dt=1;
int num_bodies=10;
const double G=6.67384 * pow(10,-11);
node *array[100001];
int index=1;
double force_x[100001]={0};
double force_y[100001]={0};

bool contains(node *,body *);
node *insert(node *, body *);
void compute(node *);
double add_mass(node *);
double add_moment_x(node *);
double add_moment_y(node *);
void print(node *);
void print_cm(node *);
//Distance
double distance(COM *,COM *);
//Theta
bool isbody(double,double);
//Force
void force_start(node *);
void force(node *,node *,double *,double *);
//Update
node *update_tree(node *);

int main()
{
    cin>>times>>num_bodies>>filename;
    node *ce=new node(100000,0,0);
    /*
    velocity *vel1=new velocity(0,0);
    acceleration *acc1=new acceleration(0,0);
    body *a=new body(1,1,1,vel1,acc1);
    velocity *vel2=new velocity(0,0);
    acceleration *acc2=new acceleration(0,0);
    body *b=new body(1,1,3,vel2,acc2);
    velocity *vel3=new velocity(0,0);
    acceleration *acc3=new acceleration(0,0);
    body *c=new body(1,3,1,vel3,acc3);
    velocity *vel4=new velocity(0,0);
    acceleration *acc4=new acceleration(0,0);
    body *d=new body(1,3,3,vel4,acc4);
    velocity *vel5=new velocity(0,0);
    acceleration *acc5=new acceleration(0,0);
    body *e=new body(1,0.5,0.25,vel5,acc5);
    */
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
            a[i++]=new body(1e10,atoi(line.c_str()),atoi(line2.c_str()),vel[1],acc[1]);
        }
        myfile2.close();
    }
    else cout << "Unable to open file";
    for(int i=1;i<=num_bodies;i++)
    {
        insert(ce,a[i]);
    }
    //print(ce);
    //const clock_t begin_time = clock();
    struct timeval t0,t1;
    gettimeofday(&t0,0);
    for(int o=1;o<=times;o++)
    {    
        compute(ce);
        force_start(ce);
        //#pragma omp parallel for
        for(int i=1;i<=num_bodies;i++)
        {
            //cout<<array[i]->com->cm<<" jakkas"<<endl;
            force(array[i],ce,&force_x[i],&force_y[i]);
            //cout<<force_x[i]<<" "<<force_y[i]<<" ppppp"<<endl;
        }
        cout<<endl<<endl<<endl;
        for(int i=1;i<=num_bodies;i++)
        {
            double newx=0,newy=0,newax=0,neway=0,newvx=0,newvy=0;
            newax=force_x[i]/array[i]->part->mass;
            neway=force_y[i]/array[i]->part->mass;
            array[i]->part->acc->x+=newax;
            array[i]->part->acc->y+=neway;
            array[i]->part->vel->x+=(array[i]->part->acc->x)*dt;
            array[i]->part->vel->y+=(array[i]->part->acc->y)*dt;
            array[i]->part->x+=array[i]->part->vel->x*dt;
            array[i]->part->y+=array[i]->part->vel->y*dt;
            cout<<setprecision(20)<<array[i]->part->x<<" "<<array[i]->part->y<<" "<<array[i]->part->vel->x<<" "<<array[i]->part->vel->y<<" "<<array[i]->part->acc->x<<" "<<array[i]->part->acc->y<<endl<<endl;
        }
    }
    gettimeofday(&t1, 0);
    long long elapsed = (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;//std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
    cout<<"Time Taken in milliseconds= "<<elapsed<<endl;
    //std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
   /*
    insert(ce,a);
    insert(ce,b);
    insert(ce,c);
    insert(ce,d);
    insert(ce,e);
    print(ce);
    compute(ce);
    cout<<distance(ce->com,ce->sw->com)<<" "<<isbody(2.00,ce->ne->width)<<" qwe"<<endl;
    force_start(ce);
    for(int i=1;i<=5;i++)
    {
        cout<<array[i]->com->cm<<" jakkas"<<endl;
        force(array[i],ce,&force_x[i],&force_y[i]);
        cout<<force_x[i]<<" "<<force_y[i]<<" ppppp"<<endl;
    }
    cout<<endl<<endl<<endl;
    for(int i=1;i<=5;i++)
    {
        double newx=0,newy=0,newax=0,neway=0,newvx=0,newvy=0;
        newax=force_x[i]/array[i]->part->mass;
        neway=force_y[i]/array[i]->part->mass;
        array[i]->part->acc->x+=newax;
        array[i]->part->acc->y+=neway;
        array[i]->part->vel->x+=(array[i]->part->acc->x)*dt;
        array[i]->part->vel->y+=(array[i]->part->acc->y)*dt;
        array[i]->part->x+=array[i]->part->vel->x*dt;
        array[i]->part->y+=array[i]->part->vel->y*dt;
        cout<<setprecision(20)<<array[i]->part->x<<" "<<array[i]->part->y<<" "<<array[i]->part->vel->x<<" "<<array[i]->part->vel->y<<" "<<array[i]->part->acc->x<<" "<<array[i]->part->acc->y<<endl<<endl;
    }
    cout<<"-------------------------------"<<endl;
    ce=update_tree(ce);
    compute(ce);
    */
}

bool contains(node *root,body *body1)
{
    if(root->posx<=body1->x&&body1->x<=root->posx+root->width)
        if(root->posy<=body1->y&&body1->y<=root->posy+root->width)
        {
            return true;
        }
    return false;
}

node *insert(node *root,body *body1)
{
    if(!contains(root,body1))
    {
        return NULL;
    }
    else if(root->part==NULL&&root->nw==NULL)
    {
        root->part=body1;
    }
    else if(root->nw==NULL)
    {
        double half=root->width/2;
        root->nw=new node(half,root->posx,root->posy+half);
        root->ne=new node(half,root->posx+half,root->posy+half);
        root->sw=new node(half,root->posx,root->posy);
        root->se=new node(half,root->posx+half,root->posy);
        insert(root->nw,root->part)||insert(root->ne,root->part)||insert(root->sw,root->part)||insert(root->se,root->part);

        insert(root->nw,body1)||insert(root->ne,body1)||insert(root->sw,body1)||insert(root->se,body1);

        root->part=NULL;
    }

    else if(root->nw!=NULL)
    {
        insert(root->nw,body1)||insert(root->ne,body1)||insert(root->sw,body1)||insert(root->se,body1);
    }
}

void print(node *root)
{
    if(!root)
        return;
    else if(root->part==NULL)
    {
        print(root->nw);
        print(root->ne);
        print(root->sw);
        print(root->se);
    }
    else
    {
        cout<<root->part->x<<" "<<root->part->y<<endl;
    }
}

void compute(node *root)
{
    if(!root)
    {
       // cout<<"c"<<endl;
        return;
    }
    else if(root->part==NULL&&root->nw==NULL)
    {
        //cout<<"d"<<endl;
        return;
    }
    else if(root->part)
    {
        //cout<<"a"<<endl;
        root->com=new COM(root->part->mass,root->part->x,root->part->y);
        //cout<<root->com->cm<<" "<<root->com->cx<<" "<<root->com->cy<<endl;
    }
    else
    {
        //cout<<"b"<<endl;
        compute(root->nw);
        compute(root->ne);
        compute(root->sw);
        compute(root->se);
        double sum_mass=add_mass(root->nw)+add_mass(root->ne)+add_mass(root->sw)+add_mass(root->se);
        double sum_moment_x=(add_moment_x(root->nw)+add_moment_x(root->ne)+add_moment_x(root->sw)+add_moment_x(root->se))/sum_mass;
        double sum_moment_y=(add_moment_y(root->nw)+add_moment_y(root->ne)+add_moment_y(root->sw)+add_moment_y(root->se))/sum_mass;
       // cout<<sum_mass<<" "<<sum_moment_x<<" "<<sum_moment_y<<" asddsa"<<endl;
        root->com=new COM(sum_mass,sum_moment_x,sum_moment_y);
    }
}

double add_mass(node *root)
{
    if(root->part==NULL&&root->nw==NULL)
        return 0;
    return root->com->cm;
}

double add_moment_x(node *root)
{
    if(root->part==NULL&&root->nw==NULL)
        return 0;
    return (root->com->cm)*(root->com->cx);
}

double add_moment_y(node *root)
{
    if(root->part==NULL&&root->nw==NULL)
        return 0;
    return root->com->cm*root->com->cy;
}

void print_cm(node *root)
{
        return;
    cout<<root->com->cm<<endl;
    print_cm(root->nw);
    print_cm(root->ne);
    print_cm(root->sw);
    print_cm(root->se);
}

//Distance Calculation
double distance(COM *com1,COM *com2)
{
    return sqrt(pow(com1->cx-com2->cx,2)+pow(com1->cy-com2->cy,2));
}

//Theta Calculation
bool isbody(double r,double w)
{
    if((double)w/r<=t)
        return true;
    else
        return false;
}

//Force Calculation
void force_start(node * root)
{
    if(root->nw)
    {
        force_start(root->nw);
        force_start(root->ne);
        force_start(root->sw);
        force_start(root->se);
    }
    else if(root->part)
    {
        array[index]=root;
        index++;
        return;
    }
    else
        return;
}

void force(node *obj,node *root,double *totx,double *toty)
{
    if(root==obj||(root->part==NULL&&root->nw==NULL))
    {
        return;
    }
    double dist=distance(obj->com,root->com);
    if(isbody(dist,root->width))
    {
        double force2=(G*obj->com->cm*root->com->cm)/(dist*dist);
        *totx=*totx+force2*(root->com->cx-obj->com->cx)/dist;
        *toty=*toty+force2*(root->com->cy-obj->com->cy)/dist;
        return;
    }
    else if(root->nw)
    {
        force(obj,root->nw,totx,toty);
        force(obj,root->ne,totx,toty);
        force(obj,root->sw,totx,toty);
        force(obj,root->se,totx,toty);
        return;
    }
}

node *update_tree(node *root)
{
    for(int i=1;i<=num_bodies;i++)
    {
        if(!contains(array[i],array[i]->part))
        {
            body *change=array[i]->part;
            array[i]->part=NULL;
            insert(root,change);
        }
    }
    return root;
}
