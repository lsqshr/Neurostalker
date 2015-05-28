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
#include "stackutil.h"

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
    int downsample; // 1 : downsample the image within 256*256*256; 0: keep the original image
};

void reconstruction_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu);
void downsample(V3DLONG* in_sz, V3DLONG c, unsigned char* data1d, V3DLONG * downsz, unsigned char* downdata1d);
 
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
        PARA.downsample = (paras.size() >= k+1) ? atoi(paras[k]) : 1;  k++;
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
    if (PARA.downsample == 1)
    {
        unsigned char * downdata1d = NULL;
        V3DLONG downsz[4];
        cout<<"Data size before downsample: "<<N<<","<<M<<","<<P<<endl;

        downsample(in_sz, c, data1d, downsz, downdata1d);

        cout<<"Data size after downsample: "<<in_sz[0]<<","<<in_sz[1]<<","<<in_sz[2]<<endl;
        cout<<"Saving downsampled image to test/testdata/downsample.v3draw"<<endl;
        saveImage("test/testdata/downsample.v3draw", downdata1d, downsz, V3D_UINT8);
        saveImage("test/testdata/origin.v3draw", data1d, in_sz, V3D_UINT8);
    }

    // Using the Image Operation found in vaa3d_tools/hackathon/zhi/snake_tracing/TracingCore/ in for some simple Image Processing
    IM = new ImageOperation;

    // Imcreate takes in_sz with int*
    int in_sz_int[4];

    for(int i = 0; i < 4; i++)
    {
        in_sz_int[i] = (int)in_sz[i];
    }

    cout<<"in_sz_int:\t"<<endl; 
    std::copy(in_sz_int,
       in_sz_int + sizeof(in_sz_int) / sizeof(in_sz_int[0]),
       ostream_iterator<int>(cout, "\n")); 
    cout<<"in_sz:\t"<<endl; 
    std::copy(in_sz,
       in_sz + sizeof(in_sz) / sizeof(in_sz[0]),
       ostream_iterator<V3DLONG>(cout, "\n")); 
    
    /*
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
    */
    
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
            
    return;
}

void downsample(V3DLONG *in_sz, V3DLONG c, unsigned char* data1d, V3DLONG * downsz, unsigned char * indata1d)
{
    V3DLONG N, M, P;
    N = in_sz[0];
    M = in_sz[1];
    P = in_sz[2];

    // --- Downsample the image
    V3DLONG pagesz = N*M*P;
    unsigned char *data1d_1ch;
    try {data1d_1ch = new unsigned char [pagesz];}
    catch(...)  {v3d_msg("cannot allocate memory for data1d_1ch."); return;}

    for(V3DLONG i = 0; i < pagesz; i++)
        data1d_1ch[i] = data1d[i+(c-1)*pagesz];

    Image4DSimple * p4dImageNew = 0;
    p4dImageNew = new Image4DSimple;

    if(!p4dImageNew->createImage(N,M,P,1, V3D_UINT8))
        return;

    memcpy(p4dImageNew->getRawData(), data1d_1ch, pagesz);

    indata1d = p4dImageNew->getRawDataAtChannel(0);

    in_sz[3] = 1;
    double dfactor_xy = 1, dfactor_z = 1;

    if (in_sz[0]<=256 && in_sz[1]<=256 && in_sz[2]<=256)
    {
        dfactor_z = dfactor_xy = 1;
    }
    else if (in_sz[0] >= 2*in_sz[2] || in_sz[1] >= 2*in_sz[2])
    {
        if (in_sz[2]<=256)
        {
            double MM = in_sz[0];
            if (MM<in_sz[1]) MM=in_sz[1];
            dfactor_xy = MM / 256.0;
            dfactor_z = 1;
        }
        else
        {
            double MM = in_sz[0];
            if (MM<in_sz[1]) MM=in_sz[1];
            if (MM<in_sz[2]) MM=in_sz[2];
            dfactor_xy = dfactor_z = MM / 256.0;
        }
    }
    else
    {
        double MM = in_sz[0];
        if (MM<in_sz[1]) MM=in_sz[1];
        if (MM<in_sz[2]) MM=in_sz[2];
        dfactor_xy = dfactor_z = MM / 256.0;
    }

    printf("dfactor_xy=%5.3f\n", dfactor_xy);
    printf("dfactor_z=%5.3f\n", dfactor_z);

    if (dfactor_z>1 || dfactor_xy>1)
    {
        v3d_msg("enter ds code", 0);

        V3DLONG out_sz[4];
        unsigned char * outimg=0;
        if (!downsampling_img_xyz( indata1d, in_sz, dfactor_xy, dfactor_z, outimg, out_sz))
        {
            cout<<"=== Downsample Failed!!"<<endl;
            return;
        }

        p4dImageNew->setData(outimg, out_sz[0], out_sz[1], out_sz[2], out_sz[3], V3D_UINT8);

        indata1d = p4dImageNew->getRawDataAtChannel(0);
        downsz[0] = p4dImageNew->getXDim();
        downsz[1] = p4dImageNew->getYDim();
        downsz[2] = p4dImageNew->getZDim();
        downsz[3] = p4dImageNew->getCDim();

    }

}