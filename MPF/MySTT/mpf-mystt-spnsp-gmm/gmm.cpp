//
// $Id: gmm.cpp,v 1.1.1.1 2005/05/06 01:15:31 xanguera Exp $
//
#include "gmm.h"

const char* message3 = "Number of Gaussians: ";
const int len3 = 21; // length 21
const char* message4 = "Gaussian ";
const int len4 = 9;
const char* message5 = "Weight ";
const int len5 = 7;

const char *message6 = "Feature ";
const int len6 = 8;
const char* message7 = "Mean ";
const int len7 = 5;
const char* message8 = "Var ";
const int len8 = 4;

GMM::GMM()
{
}

void GMM::Initialize(int num, int dim,int nSamples)
{
  M = num; 
  DIM = dim;
  NSAMPLES = nSamples;
  Weight = new float[M];
  gArray = new Gaussian[M];
  local_lkld = new float[M];
  for(int i=0; i<M; i++)
  {
	Weight[i] = 1.0/(float)M;
	gArray[i].Initialize(dim);
  }
}

float GMM::Log_Likelihood(float *feature)
{  
	//float res = 0.0;
	float log_lkld= MINVALUEFORMINUSLOG ;
	for(int i=0;i<M;i++)
	{
   		if(Weight[i]) 
		{
		  /*		  fprintf(stderr, "%f %f, %f\n",
			  Weight[i], lut_log(Weight[i],LOOKUP_TABLE,N_LOOKUP_SIZE),
			  gArray[i].Log_Likelihood(feature)); */
			  
      			local_lkld[i] = lut_log(Weight[i],LOOKUP_TABLE,N_LOOKUP_SIZE) + gArray[i].Log_Likelihood(feature);
			
			///			fprintf(stderr, "%f\n", local_lkld[i]);
			
      			if(isnan(local_lkld[i]) || !finite(local_lkld[i])) 
			{
				local_lkld[i] = MINVALUEFORMINUSLOG;
      			}
      			log_lkld = Log_Add(log_lkld, local_lkld[i]);
      		} 
		else 
		{
      			local_lkld[i] = MINVALUEFORMINUSLOG;
    		}
	}//for	
  	return log_lkld;
} 

void GMM::Initialize(float *data, int count)
{
  Initialize1DFloat(gArray[0].Mean, DIM);
  Initialize1DFloat(gArray[0].Var, DIM);
  for(int i=0;i<count;i++)
  {
	for(int j=0;j<DIM;j++)
	{
	  gArray[0].Mean[j] += data[i*DIM+j];
	  gArray[0].Var[j] += pow(data[i*DIM+j], 2.0);
	}  
  }
  for(int j=0;j<DIM;j++)
  {
	gArray[0].Mean[j] /= (float)count;
	gArray[0].Var[j] /= (float)(count-1);
	gArray[0].Var[j] -= ((float)(count) * gArray[0].Mean[j] * gArray[0].Mean[j])/(float)(count-1);
	// Do we really want to divide the variance????
	gArray[0].Var[j] /= (float)M;
	/*
	if(gArray[0].Var[j] < 0.1)
	  {
	    gArray[0].Var[j] = 0.1;
	  }
	*/

	if(gArray[0].Var[j] < MIN_VARIANCE)
	  {
	    gArray[0].Var[j] = MIN_VARIANCE;
	  }

  }
  gArray[0].calculateCP(); // LINE ADDED
  int gap = count/M;
  for(int i=1;i<M;i++)
  {
	memcpy(gArray[i].Mean, data+i*gap*DIM, sizeof(float)*DIM);
	memcpy(gArray[i].Var, gArray[0].Var, sizeof(float)*DIM);
	gArray[i].calculateCP(); //LINE ADDED
  }
  /* This seemed like a good idea. But made things worse...
  for(int i=0; i<M; i++)
  {
	Weight[i] = 1.0/(float)M;
  }
  */
}

// Return the log likelihood of this model on the given data
float 
GMM::Evaluate(float *data, int nSamples)
{
  float lkld = 0.0;

  for(int frame=0; frame<nSamples; frame++) {
    lkld += Log_Likelihood(&data[frame*DIM]);
  }
  return lkld;
}



GMM::~GMM()
{
  delete []gArray;
  delete []Weight;
  delete []local_lkld;
}

void GMM::Copy(GMM gmm)
{
  Initialize(gmm.M, gmm.DIM, gmm.NSAMPLES);
  memcpy(Weight, gmm.Weight, sizeof(float)*M);
  lkld = gmm.lkld;
  for(int i=0; i<M; i++)
  {
	memcpy(gArray[i].Mean, gmm.gArray[i].Mean, sizeof(float)*DIM);
	memcpy(gArray[i].Var, gmm.gArray[i].Var, sizeof(float)*DIM);
	gArray[i].CP=gmm.gArray[i].CP; //LINE ADDED
  }
}

GMM::GMM(const GMM &gmm)
{
  Initialize(gmm.M, gmm.DIM, gmm.NSAMPLES);
  memcpy(Weight, gmm.Weight, sizeof(float)*M);
  lkld = gmm.lkld;
  for(int i=0; i<M; i++)
  {
	memcpy(gArray[i].Mean, gmm.gArray[i].Mean, sizeof(float)*DIM);
	memcpy(gArray[i].Var, gmm.gArray[i].Var, sizeof(float)*DIM);
	gArray[i].CP=gmm.gArray[i].CP; //LINE ADDED
  }
}

inline float GMM::Log_Add(float log_a, float log_b)
{
  float result;
  if(log_a < log_b)
  {
	float tmp = log_a;
	log_a = log_b;
	log_b = tmp;
  }
  if((log_b - log_a) <= MINVALUEFORMINUSLOG)
  {
    return log_a;
  }
  else
   {
	result = log_a + (float)(lut_log(1.0 + (double)(exp((double)(log_b - log_a))),LOOKUP_TABLE,N_LOOKUP_SIZE));
   }
  return result;
}

void Gaussian::Initialize(int dim)
{
  DIM = dim;
  Mean = new float[DIM];
  Var = new float[DIM];
  for(int i=0;i<DIM;i++) {
    Mean[i] = 0.0;
    Var[i] = 0.0;
  }
  calculateCP(); //LINE ADDED
}

Gaussian::Gaussian()
{
}

Gaussian::~Gaussian()
{
  delete []Mean;
  delete []Var;
}

inline float Gaussian::Log_Likelihood(float *feature)
{
  //  fprintf(stderr, "cp=%f\n", CP);
  
  //float log_lkld;
  //float in_the_exp = 0.0, den = 0.0;
  float x,y=0,z;
  for(int i=0; i<DIM; i++)
  {
	x = feature[i]-Mean[i];
	z = Var[i];
	y += x*x/z;//+lut_log(2*3.141592654*z,LOOKUP_TABLE,N_LOOKUP_SIZE); LINE MODIFIED
  }
  return((float)-0.5*(y+CP)); //LINE MODIFIED
}


float GMM::Train(float *data, int nSamples, double *times)
{
  double **gamma_l_o = (double **)malloc(sizeof(double *)*M);
  double **gamma_l_o_2 = (double **)malloc(sizeof(double *)*M);
  double *gamma_l = (double *)malloc(sizeof(double)*M);       
  float total_log_lkld = -100000;
  float lkld;
  double contri;
  float prev_log_lkld= total_log_lkld*10;
  
  double startini;
  *gamma_l_o = (double *)calloc(M*DIM, sizeof(double));
  *gamma_l_o_2 = (double *)calloc(M*DIM, sizeof(double));
  for(int j=1; j<M; j++) {
    gamma_l_o[j] = gamma_l_o[j-1] + DIM;
    gamma_l_o_2[j] = gamma_l_o_2[j-1] + DIM;
  }
  double endini;
  times[2] += (endini - startini);	
  // Initialize the models on the data
  //Initialize(data,nSamples); seemed like a good idea, but scores are worse.

  for(int loop=0; loop < N_LOOPS_ADAPT3; loop++)
  //for(int loop=0; loop < 5; loop++)
  //1% increase at least
  //while(fabs((total_log_lkld - prev_log_lkld)/prev_log_lkld)>0.01)
  {
        double startiniloop;
	prev_log_lkld = total_log_lkld;
	Initialize1DDouble(gamma_l, M);
	Initialize2DDouble(gamma_l_o, M, DIM);
	Initialize2DDouble(gamma_l_o_2, M, DIM);
	total_log_lkld = 0.0;
	for(int frame=0; frame<nSamples; frame++) {
	  double startloglik;  	
 	  lkld = Log_Likelihood(&data[frame*DIM]);
	  double endloglik;
          times[4] += (endloglik - startloglik);
	  total_log_lkld += lkld;
	  for(int j=0; j<M; j++) {
	    contri = (double)(local_lkld[j]) - (double)(lkld);
	    contri = (double)(exp(contri));//convert from log to norm
	    gamma_l[j] += contri;
	    for(int fea=0; fea<DIM; fea++) {
	      gamma_l_o[j][fea] += contri*(double)(data[frame*DIM+fea]);
	      gamma_l_o_2[j][fea] += contri*(double)(data[frame*DIM+fea]*data[frame*DIM+fea]);
	    }
	  }/*for(int j=0; j<M; j++)*/
	}/*for(int frame=0; frame<nSamples; frame++)*/
        double endiniloop;
  	times[3] += (endiniloop - startiniloop);
        double startupdate;	
	
	for(int j=0; j<M; j++) {//reestimation
	  Weight[j] = (float)(gamma_l[j]/(double)(nSamples));
	  if(Weight[j] != 0.0) {
	    for(int fea=0; fea <DIM; fea++) {
	      gArray[j].Mean[fea] = (float)((double)(gamma_l_o[j][fea])/(double)(gamma_l[j]));
	      gArray[j].Var[fea] = (float)((double)(gamma_l_o_2[j][fea])/(double)(gamma_l[j]));
	      gArray[j].Var[fea] -= (float)(gArray[j].Mean[fea]*gArray[j].Mean[fea]);
	      /*
	      if(gArray[j].Var[fea] < 0.1) {
		gArray[j].Var[fea] = 0.1;
	      }
	      */

	      if(gArray[j].Var[fea] <= MIN_VARIANCE) {
		gArray[j].Var[fea] = MIN_VARIANCE;
	      }

	    }
	  gArray[j].calculateCP(); //LINE ADDED
	  }	  
	}
	double endupdate;
  	times[5] += (endupdate - startupdate);
  }/*for(int loop=0; loop < N_LOOPS_ADAPT; loop++)*/
  free(*gamma_l_o);free(*gamma_l_o_2);free(gamma_l_o);free(gamma_l_o_2);
  free(gamma_l);

  // Evaluate the newly trained model on the data
  double startfinallog;
  total_log_lkld = Evaluate(data,nSamples);
  double endfinallog;
  times[6] += (endfinallog - startfinallog);
  if(isnan(total_log_lkld) || !finite(total_log_lkld)) {
    printf("The total log likelihood inside the GMM training invalid\n");
  }
  //printf("Value: %f\n", total_log_lkld);fflush(stdout);
  return total_log_lkld;
  
}




float GMM::MAP_adaptation(float *data, int nSamples, double *times,float rel_factor)
{
  double **gamma_l_o = (double **)malloc(sizeof(double *)*M);
  double **gamma_l_o_2 = (double **)malloc(sizeof(double *)*M);
  double *gamma_l = (double *)malloc(sizeof(double)*M);  
	
  float total_log_lkld = -100000;
  float lkld;
  double contri;
  float prev_log_lkld= total_log_lkld*10;
  float prev_mean;
	float norm_weight=0.0;
	double alpha_wj,alpha_mj,alpha_vj;
  double startini;
  *gamma_l_o = (double *)calloc(M*DIM, sizeof(double));
  *gamma_l_o_2 = (double *)calloc(M*DIM, sizeof(double));
  for(int j=1; j<M; j++) {
    gamma_l_o[j] = gamma_l_o[j-1] + DIM;
    gamma_l_o_2[j] = gamma_l_o_2[j-1] + DIM;
  }
  double endini;
  times[2] += (endini - startini);	
  // Initialize the models on the data
  //Initialize(data,nSamples); seemed like a good idea, but scores are worse.

	//
        double startiniloop;
	prev_log_lkld = total_log_lkld;
	Initialize1DDouble(gamma_l, M);
	Initialize2DDouble(gamma_l_o, M, DIM);
	Initialize2DDouble(gamma_l_o_2, M, DIM);
	total_log_lkld = 0.0;
	
	for(int frame=0; frame<nSamples; frame++) {
	  double startloglik;  	
 	  lkld = Log_Likelihood(&data[frame*DIM]);
	  double endloglik;
          times[4] += (endloglik - startloglik);
	  total_log_lkld += lkld;
	  for(int j=0; j<M; j++) {
	    contri = (double)(local_lkld[j]) - (double)(lkld);
	    contri = (double)(exp(contri));//convert from log to norm
	    gamma_l[j] += contri;
	    for(int fea=0; fea<DIM; fea++) {
	      gamma_l_o[j][fea] += contri*(double)(data[frame*DIM+fea]);
	      gamma_l_o_2[j][fea] += contri*(double)(data[frame*DIM+fea]*data[frame*DIM+fea]);
	    }
	  }/*for(int j=0; j<M; j++)*/
	}/*for(int frame=0; frame<nSamples; frame++)*/
  double endiniloop;
  times[3] += (endiniloop - startiniloop);
  double startupdate;	
	
	for(int j=0; j<M; j++) {
	  
		alpha_wj=alpha_mj=alpha_vj=gamma_l[j]/(gamma_l[j]+rel_factor);
	
		
		Weight[j] = (float)(alpha_wj*gamma_l[j]/(double)(nSamples)+(1-alpha_wj)*Weight[j]);	  
		norm_weight+=Weight[j];
		
		
		if(Weight[j] != 0.0) {
	    for(int fea=0; fea <DIM; fea++) {
				prev_mean=gArray[j].Mean[fea];
	      gArray[j].Mean[fea] = (float) (alpha_mj*gamma_l_o[j][fea]/gamma_l[j]+(1-alpha_mj)*prev_mean) ;
				gArray[j].Var[fea] = (float)( alpha_vj*gamma_l_o_2[j][fea]/gamma_l[j]+(1-alpha_vj)*(gArray[j].Var[fea]+prev_mean*prev_mean) );
				gArray[j].Var[fea] -= (float)(gArray[j].Mean[fea]*gArray[j].Mean[fea]);
	      /*
	      if(gArray[j].Var[fea] < 0.1) {
		gArray[j].Var[fea] = 0.1;
	      }
	      */

	      if(gArray[j].Var[fea] <= MIN_VARIANCE) {
		gArray[j].Var[fea] = MIN_VARIANCE;
	      }

	    }
	  //gArray[j].calculateCP(); //LINE ADDED
	  }	  
	}
	for(int j=0; j<M; j++) {
		Weight[j]/=norm_weight;
	}
	
	double endupdate;
  times[5] += (endupdate - startupdate);
		
  free(*gamma_l_o);free(*gamma_l_o_2);free(gamma_l_o);free(gamma_l_o_2);
  free(gamma_l);

  // Evaluate the newly trained model on the data
  double startfinallog;
  total_log_lkld = Evaluate(data,nSamples);
  double endfinallog;
  times[6] += (endfinallog - startfinallog);
  if(isnan(total_log_lkld) || !finite(total_log_lkld)) {
    printf("The total log likelihood inside the GMM training invalid\n");
  }
  //printf("Value: %f\n", total_log_lkld);fflush(stdout);
  return total_log_lkld;
  
}



float GMM::MAP_means_adaptation(float *data, int nSamples, double *times,float rel_factor)
{
  double **gamma_l_o = (double **)malloc(sizeof(double *)*M);
  double **gamma_l_o_2 = (double **)malloc(sizeof(double *)*M);
  double *gamma_l = (double *)malloc(sizeof(double)*M);  
	
  float total_log_lkld = -100000;
  float lkld;
  double contri;
  float prev_log_lkld= total_log_lkld*10;
  float prev_mean;
	float norm_weight=0.0;
	double alpha_wj,alpha_mj,alpha_vj;
  double startini;
  *gamma_l_o = (double *)calloc(M*DIM, sizeof(double));
  *gamma_l_o_2 = (double *)calloc(M*DIM, sizeof(double));
  for(int j=1; j<M; j++) {
    gamma_l_o[j] = gamma_l_o[j-1] + DIM;
    gamma_l_o_2[j] = gamma_l_o_2[j-1] + DIM;
  }
  double endini;
  times[2] += (endini - startini);	
  // Initialize the models on the data
  //Initialize(data,nSamples); seemed like a good idea, but scores are worse.

	//
        double startiniloop;
	prev_log_lkld = total_log_lkld;
	Initialize1DDouble(gamma_l, M);
	Initialize2DDouble(gamma_l_o, M, DIM);
	Initialize2DDouble(gamma_l_o_2, M, DIM);
	total_log_lkld = 0.0;
	
	for(int frame=0; frame<nSamples; frame++) {
	  double startloglik;  	
 	  lkld = Log_Likelihood(&data[frame*DIM]);
	  double endloglik;
          times[4] += (endloglik - startloglik);
	  total_log_lkld += lkld;
	  for(int j=0; j<M; j++) {
	    contri = (double)(local_lkld[j]) - (double)(lkld);
	    contri = (double)(exp(contri));//convert from log to norm
	    gamma_l[j] += contri;
	    for(int fea=0; fea<DIM; fea++) {
	      gamma_l_o[j][fea] += contri*(double)(data[frame*DIM+fea]);
	      gamma_l_o_2[j][fea] += contri*(double)(data[frame*DIM+fea]*data[frame*DIM+fea]);
	    }
	  }/*for(int j=0; j<M; j++)*/
	}/*for(int frame=0; frame<nSamples; frame++)*/
  double endiniloop;
  times[3] += (endiniloop - startiniloop);
  double startupdate;	
	
	for(int j=0; j<M; j++) {
	  
		alpha_wj=alpha_mj=alpha_vj=(double)(gamma_l[j]/(gamma_l[j]+rel_factor));
	
		//Weight[j] = (float)(alpha_wj*gamma_l[j]/(double)(nSamples)+(1-alpha_wj)*Weight[j]);	  
	//	norm_weight+=Weight[j];
		
	
		if(Weight[j] != 0.0) {
	    for(int fea=0; fea <DIM; fea++) {
				
				prev_mean=gArray[j].Mean[fea];
				
	      gArray[j].Mean[fea] = (float) (alpha_mj*gamma_l_o[j][fea]/gamma_l[j]+(1-alpha_mj)*prev_mean) ;
				
				//gArray[j].Var[fea] = (float)( alpha_vj*gamma_l_o_2[j][fea]/gamma_l[j]+(1-alpha_vj)*(gArray[j].Var[fea]+prev_mean*prev_mean) );
				//gArray[j].Var[fea] -= (float)(gArray[j].Mean[fea]*gArray[j].Mean[fea]);
	      /*
	      if(gArray[j].Var[fea] < 0.1) {
		gArray[j].Var[fea] = 0.1;
	      }
	      */

	      //if(gArray[j].Var[fea] <= MIN_VARIANCE) {
		//gArray[j].Var[fea] = MIN_VARIANCE;
	    //  }

	    }
	  //gArray[j].calculateCP(); //LINE ADDED
	  }	  
	}
	//for(int j=0; j<M; j++) {
  //		Weight[j]/=norm_weight;
	//}
	
	double endupdate;
  times[5] += (endupdate - startupdate);
		
  free(*gamma_l_o);free(*gamma_l_o_2);free(gamma_l_o);free(gamma_l_o_2);
  free(gamma_l);

  // Evaluate the newly trained model on the data
  double startfinallog;
  total_log_lkld = Evaluate(data,nSamples);
  double endfinallog;
  times[6] += (endfinallog - startfinallog);
  if(isnan(total_log_lkld) || !finite(total_log_lkld)) {
    printf("The total log likelihood inside the GMM training invalid\n");
  }
  //printf("Value: %f\n", total_log_lkld);fflush(stdout);
  return total_log_lkld;
  
}




void Gaussian::calculateCP(void)
{
  //  fprintf(stderr, "DIM=%d\n", DIM);
  
	CP = 0.0;
	for(int i=0;i<DIM;i++)
	{
	  //	  fprintf(stderr, "Var[%d] = %f\n", i, Var[i]);
	  
	  CP += lut_log(2*3.141592654*Var[i],LOOKUP_TABLE,N_LOOKUP_SIZE);
	  //	  fprintf(stderr, "calculateCP loop %d=%f\n", i, CP);
	}
	//	fprintf(stderr, "calculateCP=%f\n", CP);

}

void 
GMM::PrintGMM(FILE *fout) {
  fprintf(fout, "%s%d\n", message3, M); // "Number of Gaussians: "
  for(int i = 0; i < M; i++) { // for each Gaussian
    fprintf(fout, "%s%d\n%s%f\n", message4, i, message5, Weight[i]); // "Gaussian " and "Weight "
    gArray[i].PrintGaussian(fout);
  }
}

void Gaussian::PrintGaussian(FILE *fout) {
  for (int i = 0; i < DIM; i++) { // for each feature
    fprintf(fout, "%s%d\n%s%f\n%s%f\n", message6, i, message7, Mean[i], message8, Var[i]); // "Feature ", "Mean ", and "Var "
  }
}

void
GMM::ReadGMM(FILE *fin, const int d) {
  DIM = d; // dimension of feature vector

  const int nBuffer = 30;
  char buffer[nBuffer];
  //char temp1[nBuffer];
  //char *temp2;

  // "Number of Gaussians: "
  //fgets(buffer,nBuffer,fin); // get line
  fgets(buffer,len3,fin); // get label
  //printf("missingmm: %s\n",buffer);
  //strncpy(temp1,buffer,len3); temp1[len3] = '\0'; // compare substring
  //if (!strcmp(temp1,message3)) { // if not a match
  if (!strcmp(buffer,message3)) { // if not a match
    printf("Tried to read \"%sx\" and instead found \"%s\"", message3, buffer);
    exit(0);
  }
  //memcpy(temp,buffer[len3],nBuffer-len3); // convert rest of line to int
  //temp2 = &buffer[len3]; // convert rest of line to int
  fgets(buffer,nBuffer,fin); // convert rest of line to int
  //M = atoi(temp2); // number of Gaussians

  M = atoi(buffer); // number of Gaussians
  //printf("Number of Gaussians: %d\n",M);

  //delete []gArray; gArray = new Gaussian[M]; // create new Gaussians
  //delete []Weight; Weight = new float[M]; // create new weights
  for (int a = 0; a < M; a++) { // for each Gaussian
    // "Gaussian "
    //fgets(buffer,nBuffer,fin); // get line
    fgets(buffer,len4,fin); // get label
    //strncpy(temp,buffer,len4); temp[len4] = '\0'; // compare substring
    //if (!strcmp(temp,message4)) { // if not a match
    if (!strcmp(buffer,message4)) { // if not a match
      printf("Tried to read \"%s%d\" and instead found \"%s\"", message4, a, buffer);
      exit(0);
    }
    //memcpy(temp,buffer[len4],nBuffer-len4); // convert rest of line to int
    fgets(buffer,nBuffer,fin); // convert rest of line to int
    //int b = atoi(temp); // Gaussian number
    int b = atoi(buffer); // Gaussian number
    if (b != a) { // if not the right Gaussian
      printf("Tried to read \"%s%d\" and instead found \"%s\"", message4, a, buffer);
      exit(0);
    }
    
    // "Weight "
    //fgets(buffer,nBuffer,fin); // get line
    fgets(buffer,len5,fin); // get label
    //strncpy(temp,buffer,len5); temp[len5] = '\0'; // compare substring
    //if (!strcmp(temp,message5)) { // if not a match
    if (!strcmp(buffer,message5)) { // if not a match
      printf("Tried to read \"%sx\" and instead found \"%s\"", message5, buffer);
      exit(0);
    }
    //memcpy(temp,buffer[len5],nBuffer-len5); // convert rest of line to float
    fgets(buffer,nBuffer,fin); // convert rest of line to float
    //Weight[a] = atof(temp); // weight of Gaussian
    Weight[a] = atof(buffer); // weight of Gaussian
    
    gArray[a].ReadGaussian(fin, DIM); // read in Gaussians
  }
}

void
Gaussian::ReadGaussian(FILE *fin, const int d) {
  DIM = d; // dimension of feature vector

  const int nBuffer = 30;
  char buffer[nBuffer];
  //char temp[nBuffer];

 // delete []Mean; Mean = new float[DIM]; // create new Mean vector
 // delete []Var; Var = new float[DIM]; // create new Var vector

  for (int a = 0; a < DIM; a++) {
    // "Feature "
    //fgets(buffer,nBuffer,fin); // get line
    fgets(buffer,len6,fin); // get label
    //strncpy(temp,buffer,len6); temp[len6] = '\0'; // compare substring
    //if (!strcmp(temp,message6)) { // if not a match
    if (!strcmp(buffer,message6)) { // if not a match
      printf("Tried to read \"%s%d\" and instead found \"%s\"", message6, a, buffer);
      exit(0);
    }
    //memcpy(temp,buffer[len6],nBuffer-len6); // convert rest of line to int
    fgets(buffer,nBuffer,fin); // convert rest of line to int
    //int b = atoi(temp); // feature number
    int b = atoi(buffer); // feature number
    if (b != a) { // if not the right feature
      printf("Tried to read \"%s%d\" and instead found \"%s\"", message6, a, buffer);
      exit(0);
    }

    // "Mean "
    //fgets(buffer,nBuffer,fin); // get line
    fgets(buffer,len7,fin); // get line
    //strncpy(temp,buffer,len7); temp[len7] = '\0'; // compare substring
    //if (!strcmp(temp,message7)) { // if not a match
    if (!strcmp(buffer,message7)) { // if not a match
      printf("Tried to read \"%sx\" and instead found \"%s\"", message7, buffer);
      exit(0);
    }
    //memcpy(temp,buffer[len7],nBuffer-len7); // convert rest of line to float
    fgets(buffer,nBuffer,fin); // convert rest of line to float
    //Mean[a] = atof(temp); // Mean of feature
    Mean[a] = atof(buffer); // Mean of feature
		//printf("Gaussian::ReadGaussian mean %f\n",Mean[a]);
    // "Var "
    /*
    fgets(buffer,nBuffer,fin); // get line
    strncpy(temp,buffer,len8); temp[len8] = '\0'; // compare substring
    if (!strcmp(temp,message8)) {
      printf("Tried to read \"%sx\" and instead found \"%s\"", message8, buffer);
      exit(0);
    }
    memcpy(temp,buffer[len8],nBuffer-len8); // convert rest of line to float
    Var[a] = atof(temp); // Mean of feature
    */

    fgets(buffer,len8,fin); // get line
    if (!strcmp(buffer,message8)) { // if not a match
      printf("Tried to read \"%sx\" and instead found \"%s\"", message8, buffer);
      exit(0);
    }
    fgets(buffer,nBuffer,fin); // convert rest of line to float
    Var[a] = atof(buffer); // Var of feature
  }
  //  fprintf(stderr, "reading gaussian\n");
  //  PrintGaussian(stderr);
  calculateCP();
}

/*inline float lut_log (float val, float *lookup_table, int n)
{
   int *const     exp_ptr = ((int*)&val);
   int            x = *exp_ptr;
   const int      log_2 = ((x >> 23) & 255) - 127;
   x &= 0x7FFFFF;
   x = x >> (23-n);
   val=lookup_table[x];
   return ((val + log_2)* 0.69314718);

   }*/

inline float lut_log(float val, float* lut, int n) {
  return log(val);
}

