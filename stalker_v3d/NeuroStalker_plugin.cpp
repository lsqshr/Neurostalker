/* NeuroStalker_plugin.cpp
 * 
 * 2015-4-25 : by Siqi Liu, Donghao Zhang
 * WIP!
 */
 
#include <iostream>
#include <vector>
#include "v3d_message.h"
#include "basic_surf_objs.h"
#include "utils/vn_imgpreprocess.h"

#include "NeuroStalker_plugin.h"
#include "lib/ImageOperation.h"
#include "stackutil.h"
//#include "../../v3d_main/basic_c_fun/basic_memory.cpp"//note: should not include .h file, since they are template functions

ImageOperation *IM;

Q_EXPORT_PLUGIN2(NeuroStalker, NeuroStalker);

using namespace std;

struct input_PARA
{
    QString inimg_file;
    V3DLONG channel;
};

void reconstruction_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu);
void cropfunc(const V3DLONG in_sz[4], unsigned char *data1d, V3DLONG sz_img_crop[4], unsigned char *p_img8u_crop);
QStringList NeuroStalker::menulist() const
{
	return QStringList() 
		<<tr("tracing_menu")
		<<tr("about");
}

QStringList NeuroStalker::funclist() const
{
	return QStringList()
		<<tr("tracing_func")
		<<tr("help");
}

void NeuroStalker::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
	if (menu_name == tr("tracing_menu"))
	{
        bool bmenu = true;
        input_PARA PARA;
        reconstruction_func(callback,parent,PARA,bmenu);

	}
	else
	{
		v3d_msg(tr("A learning-based tracing algorithm. "
			"Developed by Siqi Liu, Donghao Zhang, 2015-4-25"));
	}
}

bool NeuroStalker::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	if (func_name == tr("tracing_func"))
	{
        bool bmenu = false;
        input_PARA PARA;

        vector<char*> * pinfiles = (input.size() >= 1) ? (vector<char*> *) input[0].p : 0;
        vector<char*> * pparas = (input.size() >= 2) ? (vector<char*> *) input[1].p : 0;
        vector<char*> infiles = (pinfiles != 0) ? * pinfiles : vector<char*>();
        vector<char*> paras = (pparas != 0) ? * pparas : vector<char*>();

        if(infiles.empty())
        {
            fprintf (stderr, "Need input image. \n");
            return false;
        }
        else
            PARA.inimg_file = infiles[0];
        int k=0;
        PARA.channel = (paras.size() >= k+1) ? atoi(paras[k]) : 1;  k++;
        reconstruction_func(callback,parent,PARA,bmenu);
	}
    else if (func_name == tr("help"))
    {

        ////HERE IS WHERE THE DEVELOPERS SHOULD UPDATE THE USAGE OF THE PLUGIN

		printf("**** Usage of NeuroStalker tracing **** \n");
		printf("vaa3d -x NeuroStalker -f tracing_func -i <inimg_file> -p <channel> <other parameters>\n");
        printf("inimg_file       The input image\n");
        printf("channel          Data channel for tracing. Start from 1 (default 1).\n");

        printf("outswc_file      Will be named automatically based on the input image file name, so you don't have to specify it.\n\n");

	}
	else return false;

	return true;
}

void reconstruction_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu)
{
    cout<<"Welcome to NeuroStalker!!"<<endl;
    unsigned char* data1d = 0;
    V3DLONG N,M,P,sc,c;
    V3DLONG in_sz[4];
    if(bmenu)
    {
        v3dhandle curwin = callback.currentImageWindow();
        if (!curwin)
        {
            QMessageBox::information(0, "", "You don't have any image open in the main window.");
            return;
        }

        Image4DSimple* p4DImage = callback.getImage(curwin);

        if (!p4DImage)
        {
            QMessageBox::information(0, "", "The image pointer is invalid. Ensure your data is valid and try again!");
            return;
        }


        data1d = p4DImage->getRawData();
        N = p4DImage->getXDim();
        M = p4DImage->getYDim();
        P = p4DImage->getZDim();
        sc = p4DImage->getCDim();

        bool ok1;

        if(sc==1)
        {
            c=1;
            ok1=true;
        }
        else
        {
            c = QInputDialog::getInteger(parent, "Channel",
                                             "Enter channel NO:",
                                             1, 1, sc, 1, &ok1);
        }

        if(!ok1)
            return;

        in_sz[0] = N;
        in_sz[1] = M;
        in_sz[2] = P;
        in_sz[3] = sc;


        PARA.inimg_file = p4DImage->getFileName();
        if(p4DImage) {delete p4DImage;};
    }
    else
    {
        int datatype = 0;
        if (!simple_loadimage_wrapper(callback,PARA.inimg_file.toStdString().c_str(), data1d, in_sz, datatype))
        {
            fprintf (stderr, "Error happens in reading the subject file [%s]. Exit. \n",PARA.inimg_file.toStdString().c_str());
            return;
        }
        if(PARA.channel < 1 || PARA.channel > in_sz[3])
        {
            fprintf (stderr, "Invalid channel number. \n");
            return;
        }
        N = in_sz[0];
        M = in_sz[1];
        P = in_sz[2];
        sc = in_sz[3];
        c = PARA.channel;
    }

    // Main neuron reconstruction code
//-----------------------------------------------------------------------------------------
    V3DLONG sz_img_crop[4];
    unsigned char *p_img8u_crop = 0;
cropfunc(in_sz, data1d, sz_img_crop, p_img8u_crop);    

//-----------------------------------------------------------------------------------------
/*
    data1d = p_img8u_crop;
    // Using the Image Operation found in vaa3d_tools/hackathon/zhi/snake_tracing/TracingCore/ in for some simple Image Processing
    IM = new ImageOperation;

    // Imcreate takes in_sz with int*
    int in_sz_int[4];

    for(int i = 0; i < 4; i++)
    {
        in_sz_int[i] = (int)sz_img_crop[i];
    }

    cout<<"in_sz_int:\t"<<endl; 
    std::copy(in_sz_int,
       in_sz_int + sizeof(in_sz_int) / sizeof(in_sz_int[0]),
       ostream_iterator<int>(cout, "\n")); 
    cout<<"in_sz:\t"<<endl; 
    std::copy(in_sz,
       in_sz + sizeof(in_sz) / sizeof(in_sz[0]),
       ostream_iterator<V3DLONG>(cout, "\n")); 
    
    IM->Imcreate(data1d, in_sz_int);
    // Preprocessing
    std::cout<<"=== Compute Gradient Vector Flow..."<<std::endl;
    IM->computeGVF(1000, 5, 1);
    std::cout<<"=== Compute Vesselness (CPU)..."<<std::endl;
    IM->ComputeGVFVesselness();
    std::cout<<"=== Detect Seed Points..."<<std::endl;
    IM->SeedDetection(IM->v_threshold, 0, 0);
    std::cout<<"=== Adjust Seed Points..."<<std::endl;
    IM->SeedAdjustment(10);
    std::cout<<"=== Preprocessing Finished..."<<std::endl;

    // Adaptive Tracing here, may replace with graph cut
    IM->ImComputeInitBackgroundModel(IM->v_threshold);
    IM->ImComputeInitForegroundModel();
    
    //Output
    NeuronTree nt;
	QString swc_name = PARA.inimg_file + "_NeuroStalker.swc";
	nt.name = "NeuroStalker";
    writeSWC_file(swc_name.toStdString().c_str(), nt);

    if(!bmenu)
    {
        if(data1d) {delete [] data1d; data1d = 0;}
        //if(p_img8u_crop) {delete [] p_img8u_crop;  p_img8u_crop = 0;}
    }

    v3d_msg(QString("Now you can drag and drop the generated swc fle [%1] into Vaa3D.").arg(swc_name.toStdString().c_str()),bmenu);
*/            
    return;
}

void cropfunc(const V3DLONG in_sz[4], unsigned char *data1d, V3DLONG sz_img_crop[4], unsigned char *p_img8u_crop)
{    
    printf("1. Find the bounding box and crop image. \n");
    V3DLONG long l_boundbox_min[3], l_boundbox_max[3];//xyz
    long l_npixels_crop;
    
    //find bounding box
    unsigned char ***p_img8u_3d = 0;
    if(!new3dpointer(p_img8u_3d ,in_sz[0], in_sz[1], in_sz[2], data1d))
    {
        printf("ERROR: Fail to allocate memory for the 4d pointer of image.\n");
        if(p_img8u_3d) {delete3dpointer(p_img8u_3d, in_sz[0], in_sz[1], in_sz[2]);}
    }
    printf("boundingbox x dimension: %d,y dimension: %d,z dimension: %d.\n", in_sz[0], in_sz[1], in_sz[2]);
    l_boundbox_min[0] = in_sz[0];  l_boundbox_min[1] = in_sz[1];  l_boundbox_min[2] = in_sz[2];
    l_boundbox_max[0] = 0;                l_boundbox_max[1] = 0;                l_boundbox_max[2] = 0;
    for(long X=0;X<in_sz[0];X++)
        for(long Y=0;Y<in_sz[1];Y++)
            for(long Z=0;Z<in_sz[2];Z++)
                if(p_img8u_3d[Z][Y][X]>0.1)
                {
                    if(l_boundbox_min[0] > X) l_boundbox_min[0] = X;    if(l_boundbox_max[0] < X) l_boundbox_max[0] = X;
                    if(l_boundbox_min[1] > Y) l_boundbox_min[1] = Y;    if(l_boundbox_max[1] < Y) l_boundbox_max[1] = Y;
                    if(l_boundbox_min[2] > Z) l_boundbox_min[2] = Z;    if(l_boundbox_max[2] < Z) l_boundbox_max[2] = Z;
                }
    printf(">>boundingbox: x[%ld~%ld],y[%ld~%ld],z[%ld~%ld]\n",l_boundbox_min[0], l_boundbox_max[0],
                                                               l_boundbox_min[1], l_boundbox_max[1],
                                                               l_boundbox_min[2], l_boundbox_max[2]);

    //crop image
    sz_img_crop[0] = l_boundbox_max[0] - l_boundbox_min[0] + 1;
    sz_img_crop[1] = l_boundbox_max[1] - l_boundbox_min[1] + 1;
    sz_img_crop[2] = l_boundbox_max[2] - l_boundbox_min[2] + 1;
    sz_img_crop[3] = 1;
    l_npixels_crop = sz_img_crop[0] * sz_img_crop[1] * sz_img_crop[2];

    p_img8u_crop = new(std::nothrow) unsigned char[l_npixels_crop]();
    if(!p_img8u_crop)
    {
        printf("ERROR: Fail to allocate memory for p_img32f_crop!\n");
        if(p_img8u_3d)              {delete3dpointer(p_img8u_3d, in_sz[0], in_sz[1], in_sz[2]);}
    }
    unsigned char *p_tmp = p_img8u_crop;
    for(long Z = 0;Z < sz_img_crop[2];Z++)
        for(long Y = 0;Y < sz_img_crop[1];Y++)
            for(long X = 0;X < sz_img_crop[0];X++)
            {
                *p_tmp = p_img8u_3d[Z+l_boundbox_min[2]][Y+l_boundbox_min[1]][X+l_boundbox_min[0]];
                p_tmp++;
            }
    if(p_img8u_3d) {delete3dpointer(p_img8u_3d, in_sz[0], in_sz[1], in_sz[2]);}
    saveImage("test/testdata/crop.v3draw", p_img8u_crop, sz_img_crop, V3D_UINT8);
    printf("success or not this time?\n");
 }   