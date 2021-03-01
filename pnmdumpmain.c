
//

/*pnmdumpmain.c*/
#include <string.h>
#include <stdio.h>
int hexdump(FILE *file);
void error();
int * first4line(FILE *image);
int write4line(int *result,FILE *outfile);
int P2toP5(FILE *image, int *result, FILE *outfile);
int P5toP2(FILE *image, int *result, FILE *outfile);
int Rotate2(FILE *image, int *result, FILE *outfile);
int Rotate5(FILE *image, int *result, FILE *outfile);
int Rotate902(FILE *image, int *result, FILE *outfile);
int Rotate905(FILE *image, int *result, FILE *outfile);
int Invert2(FILE *image, int *result, FILE *outfile);
int Invert5(FILE *image, int *result, FILE *outfile);
int main(int argc, char *argv[])
{
    char ver[]= "1.0";
	int *result;
	int check=0;
    if(argc>=2)
    {//printing the version number of the file from a variable//
		if (!strcmp(argv[1], "--version"))
		{
			fprintf(stdout, "%s\n",ver);
			return 0;
		}//printing the usage value aka the list of commands that can be used//
		else if (!strcmp(argv[1], "--usage"))
		{
			fprintf(stdout, "Usage: \n./pnmdump.exe --version \n./pnmdump.exe --usage \n./pnmdump.exe --hexdump [FILE]\n");
			return 0;
		}
		else if (!strcmp(argv[1], "--hexdump"))
		{//opens and stores the infile with a pointer//
			FILE *file;
			if(argc==3)
			{
				file= fopen(argv[2], "rb");	
			}
			if(argc<3)
			{
				//if no input file provided use standard input//
				file= stdin;
			}
			if(argc>3)	
			{
				//if there are too many arguments provided call the error function//
				error();
				return 1;
			}
			if(file==NULL)
			{
				//if the file provided does not exist respond with an error and the name of the provided file//
				fprintf(stderr,"No such file: \"%s\"\n",argv[2]);
				return 1;
			}
			hexdump(file);
		}
		
		else if(!strcmp(argv[1],"--P2toP5"))
		{
			//allocating file pointers and calling functions to check and change format//
			FILE *image;
			FILE *outfile;
			//making sure to open files in correct format to allow manipulation//
			image= fopen(argv[2],"r");
			outfile= fopen(argv[3],"wb");
			result=first4line(image);
			if(result[1]==-1 || result[2]==-1)
			{
				//if there is an error in reading the first 4 lines returns -1 for width and height hence causes an error//
				fprintf(stderr,"Corrupted input file\n");
				return 1;
			}
			else if(result[0]==2)
			{
				write4line(result,outfile);
				check=P2toP5(image,result,outfile);
				//p2top5 returns 0 if fine and 1 if there is an error or corruption in the code//
				if(check==1)
				{
					fprintf(stderr,"Corrupted input file\n");
					return 1;
				}
				
			}
			else
			{
				fprintf(stderr,"Input is not in P2 format\n");
				return 1;
			}
			
		}
		
		else if(!strcmp(argv[1],"--P5toP2"))
		{
			FILE *image;
			FILE *outfile;
			//setting up in and out files for use in our functions//
			image= fopen(argv[2],"rb");
			outfile= fopen(argv[3],"wb");
			result=first4line(image);
			if(result[0]==5)
			{
				write4line(result,outfile);
				check=P5toP2(image,result,outfile);
				if(check==1)
				{
					fprintf(stderr,"Corrupted input file\n");
					return 1;
				}
			}
			else
			{
				fprintf(stderr,"Input is not in P5 format\n");
				return 1;
			}
			
		}
		
		else if(!strcmp(argv[1],"--rotate"))
		{
			//if user requests rotate function move into this loop//]
			//check all formatting is in order before calling rotate function//
			FILE *image;
			FILE *outfile;
			image= fopen(argv[2],"rb");
			outfile= fopen(argv[3],"wb");
			result=first4line(image);
			if(result[0]==2)
			{
				//input file is in p2 format so need p2 function//
				Rotate2(image,result,outfile);
			}
			else if(result[0]==5)
			{
				//input file is in p5 format so need p5 function//
				Rotate5(image,result,outfile);
			}
			else if(result[0]!=5&&2)
			{
				fprintf(stderr,"incorrect file type");
			}
		}
		
		else if(!strcmp(argv[1],"--rotate90"))
		{
			//if user requests rotate by 90 degrees//
			FILE *image;
			FILE *outfile;
			image= fopen(argv[2],"rb");
			outfile= fopen(argv[3],"wb");
			result=first4line(image);
			if(result[0]==2)
			{
				//if in file is of p2 format call function to rotate p2//
				Rotate902(image,result,outfile);
			}
			else if(result[0]==5)
			{
				// if file is in p5 format call function to rotate p5//
				Rotate905(image,result,outfile);
			}
			else if(result[0]!=5&&2)
			{
				//if file is of incorrect format print error//
				fprintf(stderr,"incorrect file type");
			}
			
		}
		else if(!strcmp(argv[1],"--invert"))
		{
			//user can request a colour inversion function (made for extension)
			FILE *image;
			FILE *outfile;
			image= fopen(argv[2],"rb");
			outfile= fopen(argv[3],"wb");
			result=first4line(image);
			if(result[0]==2)
			{
				//if input file is p2 call p2 invert function//
				Invert2(image,result,outfile);
			}
			else if(result[0]==5)
			{
				//if input file is p5 call p5 invert function//
				Invert5(image,result,outfile);
			}
			else if(result[0]!=5&&2)
			{
				//if file format is incorrect print an error//
				fprintf(stderr,"incorrect file type");
			}
		}
		else if(argc>=3)
		{
			//if the number of arguments is too high or not valid for functionality call the error function//
			error();
			return 1;
		}
	}
	else
	{
		//if there are too few input arguments e.g. not calling any functionality then gives error//
		error();
		return 1;
	}
	return 0;

}

int hexdump(FILE *file)
{
	int lineno=0;
	int byteno=0;
	unsigned char byte[2];
	//reads the in file 8 bytes at a time into and array for later use//
	printf("%07x",lineno);
	while(fread(byte,1,1,file)>=1)
	{//%07x prints the 7 digit value in hex of the line number//
		if(((byteno)%8==0)&&(byteno!=0))
		{
			byteno++;
			printf("\n%07x",lineno);
			printf("  %02X",byte[0]);
			if(byte[0]>=32 && byte[0]<=126)
			{//%c used to print the ascii character of a byte//
				printf(" %c",byte[0]);
			}
			else
			{
				printf(" .");
			}
		}
		else
		{
			printf("  %02X",byte[0]);
			if(byte[0]>=32 && byte[0]<=126)
			{//%c used to print the ascii character of a byte//
				printf(" %c",byte[0]);
			}
			else if(byte[0]== EOF)
			{
				//continues to next line if not end of file//
				printf("\n");
				break;
			}	
			else
			{
				//if the character is not printable give a dot//
				printf(" .");
			}
			byteno++;
		}
		
		lineno=byteno;
	}
	
	printf("\n%07x\n",lineno);
	return 0;
}

void error()
{
	fprintf(stderr, "pnmdump: bad arguments\nUsage:\n./pnmdump.exe --version\n./pnmdump.exe --usage\n./pnmdump.exe --hexdump [FILE]\n");
}

int * first4line(FILE *image)
{
	int control=0;
	char data[4096];
	int format,width,height,max;
	static int result[4];
	while(control<5)
	{
		control++;
		//checking and saving the format//
		if(control==1)
		{
			fgets(data,4096,image);
			format=data[1]-'0';
			
		}
		//skipping the comment line//
		if(control==2)
		{
			fgets(data,4096,image);	
		}
		//checking and storing the width and height//
		if(control==3)
		{
			int ret=fscanf(image,"%d %d",&width,&height);
			if(ret!=2)
			{
				width=-1;
				height=-1;
			}
					
		}
		//checking and storing the max value//
		if(control==4)
		{
			fscanf(image,"%d",&max);
			
		}
		//storing all our variables into an array to be returned//
		result[0]=format;
		result[1]=width;
		result[2]=height;
		result[3]=max;
	}
	return result;
}

int write4line(int *result,FILE *outfile)
{
	//can be called to write the first 4 lines of a file to an output file//
	int format=result[0];
	int width=result[1];
	int height=result[2];
	int max=result[3];
	if(format==2)
	{
		//switches the format so when converting the file from p2-p5 the header is correct//
		format=5;
	}
	else if(format==5)
	{
		format=2;
	}
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",width,height);
	fprintf(outfile,"%d\n",max);
	return 0;
}



int P2toP5(FILE *image, int *result, FILE *outfile)
{
	//function to convert a p2 file to p5//
	int cols=result[1];
	int rows=result[2];
	//initialises a 2d array of the correct size//
	int mat[rows][cols];
	
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			//scans for integers in the p2 file and reads them into their correct position in the 2d array//
			fscanf(image,"%d",&mat[i][j]);
			if(mat[i][j]>255)
			{
				//checks for any values in the file not within the correct range//
				return 1;
			}
			else
			{
				//switches to an unsigned char array to allow us to print out to a binary file//
				unsigned char char_buffer = (unsigned char) mat[i][j];
				fwrite(&char_buffer, sizeof(char_buffer), 1, outfile);
			}
		}
	}
	return 0;
}

int P5toP2(FILE *image, int *result, FILE *outfile)
{
	//function to convert from p5-p2//
	int cols= result[1];
	int rows= result[2];
	int mat[rows][cols];
	unsigned char byte[1];
	
	//looping through and reading into the matrix//
	fgetc(image);
	//skipping a character to avoid a newline being printed//
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			if(fread(byte,1,1,image)==1)
			{
				//checks that bytes are being read if so continues to assign them to a matrix//
				mat[i][j]=byte[0];
				if(mat[i][j]>255)
				{
					return 1;
				}
				else
				{
					//printing in the out file in the correct format with spaces in the correct place//
					if(j==cols-1)
					{
						fprintf(outfile,"%d",mat[i][j]);
					}
					else
					{
						fprintf(outfile,"%d ",mat[i][j]);
					}
				}
			}
			else
			{
				return 1;
			}
			
		}
		fprintf(outfile,"\n");
	}

	if (fread(byte,1,1,image) != 0)
	//checking that the next byte is the end of the file if not call an error//
	{
		return 1;
	}
	return 0;
}

int Rotate2(FILE *image, int *result, FILE *outfile)
{
	//rotating a p2 image//
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	int mat[cols][rows];
	int byte[1];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",rows,cols);
	fprintf(outfile,"%d\n",max);
	//looping through the matrix assigning position [i][j] to position [j][i]
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			fscanf(image,"%d",&byte[0]);
			mat[j][i]=byte[0];
		}
	}
	for(int i=0;i!=cols;i++)
	{
		for(int j=0;j!=rows;j++)
		{
			//printing position [i][j] into outfile looping through the opposite way and formatting by position//
			if(j==rows-1)
			{
				fprintf(outfile,"%d",mat[i][j]);
			}
			else
			{
				fprintf(outfile,"%d ",mat[i][j]);
			}
		}
		fprintf(outfile,"\n");
	}
	return 0;
}

int Rotate5(FILE *image, int *result, FILE *outfile)
{
	//rotating a p5 image//
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	unsigned char mat[cols][rows];
	unsigned char byte[1];
	char data[250];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",rows,cols);
	fprintf(outfile,"%d\n",max);
	fgets(data,250,image);
	//looping through the matrix assigning position [i][j] to position [j][i]
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			fread(byte,1,1,image);
			mat[j][i]=byte[0];
			
		}
		
	}
	
	
	
	for(int i=0;i<cols;i++)
	{
		for(int j=0;j<rows;j++)
		{
			//printing position [i][j] into outfile//
			fprintf(outfile,"%c",mat[i][j]);
		}
	}	
	return 0;
}
int Rotate902(FILE *image, int *result, FILE *outfile)
{
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	int matr[cols][rows];
	int byte[1];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",rows,cols);
	fprintf(outfile,"%d\n",max);
	//setting up the matrix//
	for(int i=0; i!=rows; i++)
	{
		for(int j=0; j!=cols; j++)
		{
			fscanf(image,"%d",&byte[0]);
			matr[j][i]=byte[0];
		}
	}
	//manipulating the matrix//
	for(int i=0; i!=cols; i++)
	{
		for(int j=0; j<(rows/2); j++)
		{
			//reflecting the file about the middle column//
			int tmp = matr[i][j];
            matr[i][j] = matr[i][rows-1-j];
            matr[i][rows-1-j] = tmp;
		}
	}
	//writing out the matrix//
	for(int i=0;i!=cols;i++)
	{
		for(int j=0;j!=rows;j++)
		{
			if(j==rows-1)
			{
				fprintf(outfile,"%d",matr[i][j]);
			}
			else
			{
				fprintf(outfile,"%d ",matr[i][j]);
			}
		}
		fprintf(outfile,"\n");
	}
	return 0;
}

int Rotate905(FILE *image, int *result, FILE *outfile)
{
	//rotating a p2 image//
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	unsigned char mat[cols][rows];
	char data[250];
	unsigned char byte[1];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",rows,cols);
	fprintf(outfile,"%d\n",max);
	//looping through the matrix assigning position [i][j] to position [j][i]
	fgets(data,250,image);
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			//printing position [i][j] into outfile//
			fread(byte,1,1,image);
			mat[j][i]=byte[0];
		}
	}
	
	for(int i=0; i!=cols; i++)
	{
		for(int j=0; j<(rows/2); j++)
		{
			//reflecting the image about the central column//
			int tmp = mat[i][j];
            mat[i][j] = mat[i][rows-1-j];
            mat[i][rows-1-j] = tmp;
		}
	}
	
	for(int i=0;i!=cols;i++)
	{
		for(int j=0;j!=rows;j++)
		{
			fprintf(outfile,"%c",mat[i][j]);
		}
		
	}	
	return 0;
}

int Invert2(FILE *image, int *result, FILE *outfile)
{
	//function to invert the colours of the image//
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	int mat[rows][cols];
	int byte[1];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",cols,rows);
	fprintf(outfile,"%d\n",max);
	//looping through the matrix assigning position [i][j] to position [j][i]
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			//printing position [i][j] into outfile//
			fscanf(image,"%d",&byte[0]);
			mat[i][j]=byte[0];
			int tmp=mat[i][j]-255;
			if(tmp<0)
			{
				//makes sure that the values are all positive//
				tmp=tmp*(-1);
				mat[i][j]=tmp;
			}
			else
			{
				mat[i][j]=tmp;
			}
		}
	}
	for(int i=0;i!=rows;i++)
	{
		for(int j=0;j!=cols;j++)
		{
			//loops through and prints the matrix to the out file//
			if(j==cols-1)
			{
				fprintf(outfile,"%d",mat[i][j]);
			}
			else
			{
				fprintf(outfile,"%d ",mat[i][j]);
			}
		}
		fprintf(outfile,"\n");
	}
	return 0;
}

int Invert5(FILE *image, int *result, FILE *outfile)
{
	//inverting a p5 image//
	int format= result[0];
	int cols= result[1];
	int rows= result[2];
	int max= result[3];
	unsigned char mat[rows][cols];
	unsigned char byte[1];
	//outputting the format into a new file//
	fprintf(outfile,"P%d\n",format);
	fprintf(outfile,"# Generated by pnmdump.exe\n");
	fprintf(outfile,"%d %d\n",cols,rows);
	fprintf(outfile,"%d\n",max);
	fgetc(image);
	//looping through the matrix assigning position [i][j] to 255-[i][j]//
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			//printing position [i][j] into outfile//
			fread(byte,1,1,image);
			mat[i][j]=byte[0];
			//making sure it is of type unsigned char//
			unsigned char tmp=255-mat[i][j];
			mat[i][j]=tmp;
			
		}
		
	}
	
	
	
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		//looping through and printing to outfile//
		{
			fprintf(outfile,"%c",mat[i][j]);
		}
	}	
	return 0;
}
