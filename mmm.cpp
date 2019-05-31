#include<iostream>
#include<ctime>
#include<cstdlib>
#include<fstream>

using namespace std;

int const expr_time = 10 ;
int const p_size =  12 ;				// numbers of process in sequence
int const page_size = 5;				// numbers of frames
int const process_seq = 5 ;				// label each process in 0 to process_seq
int pf_fifo=0,pf_opt=0,pf_lru=0 ;

int find_page(int page[],int p){
	for(int i=0 ; i<page_size ;i++){
		if(page[i]==p){
			return i;	
		}
	}return -1;
}

void FIFO(int page[],int p[]){
	int  j=0;
	for(int i=0 ; i<p_size; i++){
		if(j>=page_size) j=0;
		int check=find_page(page,p[i]) ;
		if( (page[j]<0&&check==(-1)) || check==(-1) ){
			page[j]=p[i];
			pf_fifo++ ;
			j++;
		}
	}cout << "FIFO page_fault : " << pf_fifo << endl;
}

void optimal(int page[],int p[]){
	int max=-1,x[page_size],idx=-1;
	for(int k=0 ; k<page_size ; k++){
		page[k] = p[k] ;
		pf_opt++;
	}
	for(int i=page_size ; i<p_size;i++){
		max=-1,idx=-1;
		for(int k=0 ; k<page_size ; k++)x[k]=-1;
		if(find_page(page,p[i]) <0){
			for(int k=i ; k<p_size+1 ; k++){
				int v=find_page(page,p[k]) ;
				if(k>idx && v>=0){
					if(x[v]==-1){
						idx=k ;
						max=v ;
						x[v]=v ; 						
					}
				}
			}
			for(int k=0 ; k<page_size;k++){
				if(x[k] < 0 ){
					max=k;
				}
			}
			if(max==-1)max=0 ;
			page[max] = p[i] ;
			pf_opt++;
		}
	}cout << "OPTIMAL page_fault : " << pf_opt << endl;
}

void LRU(int page[],int p[]){
	int idx=0 ;
	int history[page_size] ;
	for(int i=0;i<page_size;i++){
		history[i]=0;
		page[i]=p[i];
		history[i]+=i ;
		pf_lru++ ;
	}
	for(int i=page_size,j=0 ; i<p_size ; i++){
		int check = find_page(page,p[i]) ;
		if(check>=0){
			history[check]=i;
		}else if(check<0){
			for(int k=0 ; k<page_size ;k++){
					int min=history[k] ;
				for(int m=0;m<page_size;m++){
					if(history[m]<min){
						idx=m;	
						min=history[m] ;
					}
				}
			}
			page[idx]=p[i];
			history[idx]=i;
			pf_lru++ ;
		}
	}
	cout << "LRU page_fault : " << pf_lru << endl;
}

int main(){
	srand (time(NULL));
	
	ofstream result;
	result.open("expr_for_4and12.csv",std::ios_base::app) ;		//,ofstream::out | ofstream::trunc
	result << "# process," <<  p_size << "\n" ;
	result << "# page," <<  page_size << "\n" ;
	result << "Page fault occurrence\n" ;
	result << "Round,FIFO,OPT,LRU\n" ;
	int sum_fifo=0 , sum_opt=0 , sum_lru=0 ;
	
	for(int count=0 ; count<expr_time ;count++){
		pf_fifo=0 ; pf_opt=0 ; pf_lru=0 ;
		int  p[p_size] , page[page_size] ;
		//-------------------- string for Belady anomaly test ------------------
		int def_p[p_size] = {0,1,2,3,1,2,4,0,1,2,3,4} ;//  {0,1,2,3,3,2,1,4,0,1,2,4};
		//int def_p[p_size] = {1,2,1,2,3,1,2,1,2,3,3};
		//int def_p[p_size] = {1,2,1,2,3,3,1,2,1,2,3};
		//int def_p[p_size] = {2,1,2,1,2,3,3,3,1,2,1};
		//int def_p[p_size] = {3,3,1,2,1,2,1,2,1,2,3};
		//int def_p[p_size] = {1,2,1,2,1,3,3,2,1,2,3};
		//----------------------------------------------------------------------
		//-------------------- string type 1
		//int def_p[p_size] ;
		for(int i=0 ; i<p_size ;i++){
			int x= rand()%process_seq;
			while(x==def_p[i-1])x=rand()%process_seq;
			def_p[i]=x ;	
			//cout << x << " " ;
		}// cout << endl;
		
		// ------------------- string type 2
		//int def_p[p_size] ;
		/*int  prev=-1 , rerand;
		for(int i=0 ; i<p_size ; i++){
			int x=rand()%process_seq;
			bool flag[process_seq] ,abb=true;
			if(i%process_seq==0){
				for(int k=0;k<process_seq;k++)flag[k]=false;
			}
			if(flag[x] || rerand==prev){	// this set already has this number
					rerand=rand()%process_seq ;
				while(abb){
					if(flag[rerand] || rerand==prev)rerand=rand()%process_seq ;
					else abb=false;
				}
				def_p[i]=rerand ;
				flag[rerand]=true ;
				prev=rerand;
			}else{
				def_p[i]=x;
				flag[x]=true;		
				prev=x;			
			}		
			//cout << p[i] << " " ;
		}//cout << endl;*/

		//----------------------------------------------------------
		
		
{
		for(int i=0 ; i<page_size ; i++) page[i] = -1 ;
		for(int i=0 ; i<p_size ; i++) p[i] = def_p[i] ;
}
		FIFO(page,p) ;
{
		for(int i=0 ; i<page_size ; i++) page[i] = -1 ;
		for(int i=0 ; i<p_size ; i++) p[i] = def_p[i] ;	
}
		optimal(page,p);
{
		for(int i=0 ; i<page_size ; i++) page[i] = -1 ;
		for(int i=0 ; i<p_size ; i++) p[i] = def_p[i] ;	
}
		LRU(page,p);
		
		sum_fifo+=pf_fifo ;
		sum_opt+=pf_opt;
		sum_lru+=pf_lru ;

		// --------------- to csv file ---------------
		result << count+1 << "," << pf_fifo <<","<<pf_opt<<","<<pf_lru <<endl  ;
		//--------------------------------------------
		cout  << endl;
	}
		result << "AVG : ,"<< sum_fifo/expr_time<<","<<sum_opt/expr_time<<","<<sum_lru/expr_time<<"\n" ;	
		result.close() ;

}
