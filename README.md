##WSRec Benchmarks

This repository maintains a suite of benchmarks of the state-of-the-art QoS prediction approaches for Web service recommendation.

Read more information: [Paper (coming soon)][[Project page](http://wsdream.github.io/WSRec)]


##Citation

If YOU USE ANY BENCHMARK IN PUBLISHED RESEARCH, PLEASE KINDLY **CITE** THE FOLLOWING PAPER. THANKS!

- Jieming Zhu, Pinjia He, Zibin Zheng, and Michael R. Lyu, "Benchmarking and Improving QoS Prediction Approaches for Web Service Recommendation," available at: http://wsdream.github.io/WSRec.


##Related Links
- Paper list: http://wsdream.github.io/WSRec/paperlist

- QoS datasets: http://wsdream.github.io/dataset


##Code Archive
Please visit [this page for a summary of evaluation results](http://wsdream.github.io/WSRec/evaluation_result.pdf) and [this page for detailed change logs](https://github.com/WS-DREAM/WSRec/releases).

- Basic approaches (neighbourhood-based and model-based)
  - [UIPCC](https://github.com/WS-DREAM/WSRec/tree/master/UIPCC) [Zheng et al., [ICWS'09](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5175854&tag=1), [TSC'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5674010)] \(a.k.a. WSRec, including UMEAN, IMEAN, UPCC, IPCC)
  - [PMF](https://github.com/WS-DREAM/WSRec/tree/master/PMF) [Salakhutdinov et al., NIPS'07][Zheng et al., [TSC'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6122009)] \(a.k.a. Regularized SVD or RSVD)
  - [NMF](https://github.com/WS-DREAM/WSRec/tree/master/NMF) [Lee et al., NIPS'00][Zhang et al., [SRDS'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6076756)]
  - [Biased_MF](https://github.com/WS-DREAM/WSRec/tree/master/Biased_MF) [Koren et al., KDD'08][Yu et al., [SCC'14](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=6930523)]
  - [CloudPred](https://github.com/WS-DREAM/WSRec/tree/master/CloudPred) [Zhang et al., [SRDS'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6076756)]
  - [EMF](https://github.com/WS-DREAM/WSRec/tree/master/EMF) [Lo et al., [SCC'12](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6274140)]
  - [ADF](https://github.com/WS-DREAM/WSRec/tree/master/ADF) [Wu et al., [TSMC'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6301755)]
  - [NRCF](https://github.com/WS-DREAM/WSRec/tree/master/NRCF) [Sun et al., [TSC'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6338940&tag=1)]
  - [NIMF](https://github.com/WS-DREAM/WSRec/tree/master/NIMF) [Zheng et al., [TSC'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6122009)]
  - [LN_LFM](https://github.com/WS-DREAM/WSRec/tree/master/LN_LFM) [Yu et al., [SCC'14](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=6930523)]

- Location-aware
  - [RegionKNN](https://github.com/WS-DREAM/WSRec/tree/master/Location-aware/RegionKNN) [Chen et al., [ICWS'10](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5552807)]
  - [LACF](https://github.com/WS-DREAM/WSRec/tree/master/Location-aware/LACF) [Tang et al., [ICWS'12](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6257808)]
  - [LBR](https://github.com/WS-DREAM/WSRec/tree/master/Location-aware/LBR) [Lo et al., [ICWS'12](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6257841)]
  - [HMF](https://github.com/WS-DREAM/WSRec/tree/master/Location-aware/HMF) [He et al., ICWS'14]
  - [LoRec](https://github.com/WS-DREAM/WSRec/tree/master/Location-aware/LoRec) [Chen et al., [TPDS'14](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6684151)]

- Time-aware
  - [UIPCC](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/UIPCC) [Zheng et al., [ICWS'09](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5175854&tag=1), [TSC'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5674010)] \(a.k.a. WSRec, including UMEAN, IMEAN, UPCC, IPCC) (customized for time-aware dataset#2)
  - [PMF](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/PMF) [Salakhutdinov et al., NIPS'07][Zheng et al., [TSC'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6122009)] \(a.k.a. Regularized SVD or RSVD) (customized for time-aware dataset#2)
  - [TF](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/TF) [Zhang et al., [ISSRE'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6132969&tag=1)]
  - [WSPred](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/WSPred) [Zhang et al., [ISSRE'11](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6132969&tag=1)]
  - [CLUS](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/CLUS) [Silic et al., [FSE'13](http://dl.acm.org/citation.cfm?id=2491424), [TSC'15](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=6874541)]  
  - [NTF](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/NTF) [Zhang et al., [WWW'14](http://dl.acm.org/citation.cfm?id=2568001)]
  - [TD-WSRec](https://github.com/WS-DREAM/WSRec/tree/master/Time-aware/TD-WSRec) [Hu et al., [ICWS'14](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=6928878)] \(under testing, coming soon...now available upon request)
  
- Online-prediction
  - [AMF](https://github.com/WS-DREAM/WSRec/tree/master/Online-prediction/AMF) [Zhu et al., [ICDCS'14](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6888908&tag=1)] \(under testing, coming soon...) ([an early version in Matlab](https://github.com/WS-DREAM/AMF_pack))
  - [OPred](https://github.com/WS-DREAM/WSRec/tree/master/Online-prediction/OPred) [Zhang et al., [TSMC'14](http://ieeexplore.ieee.org/xpl/articleDetails.jsp?arnumber=6720144)] \(under testing, coming soon...now available upon request)

- Ranking-based
  - [GreedyRank](https://github.com/WS-DREAM/WSRec/tree/master/Ranking-based/GreedyRank) [Zheng et al., [SRDS'10](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5623393)] \(now available upon request)
  - [CloudRank](https://github.com/WS-DREAM/WSRec/tree/master/Ranking-based/CloudRank) [Zheng et al., [SRDS'10](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5623393), [TPDS'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6320550)] \(now available upon request)



##Dependencies
- Python 2.7 (https://www.python.org)
- Cython 0.20.1 (http://cython.org)
- numpy 1.8.1 (http://www.scipy.org)
- scipy 0.13.3 (http://www.scipy.org)

The benchmarks are implemented with a combination of Python and C++. The framework is built on Python for simplicity, and the core functions of each algorithm are written in C++ for efficiency consideration. To achieve so, [Cython](http://cython.org/ "Cython's Web page") (a language to write C/C++ extensions for Python) has been employed to compile the C++ extensions to Python-compatible modules. In our repository, Cython (with version 0.20.1) has been set as a sub-module in the "externals/Cython" directory.

>Note: Our code is directly executable on Linux platform. Re-compilation with Cython is required to execute them on Windows platform. 



##Usage
1. Download this code repository:
    
	Click the following link: [https://github.com/WS-DREAM/WSRec/archive/master.zip](https://github.com/WS-DREAM/WSRec/archive/master.zip)

    or use Git: `git clone https://github.com/WS-DREAM/WSRec.git`
	
	then initialize the submodules by executing: `git submodule update --init --recursive`
  
    >The repository has maintained the common evaluation results that are ready for immediate use. If you need to further customize any approach, please go through the following steps.
2. Install Cython (with Linux):
	```
	# change YOUR_DIRECTORY to your working path  
    vim ~/.bash_profile  
    export PYTHONPATH="$PYTHONPATH:YOUR_DIRECTORY/WSRec/externals/Cython/"  
    export PATH="$PATH:YOUR_DIRECTORY/WSRec/externals/Cython/bin/"  
    ```  
3. Read `"readme.txt"` for each appraoch, and execute the provided scripts (with configurable parameters):
    
	```
    python run_rt.py  
    python run_tp.py  
    ```
4. Check the evaluation results in "result/" directory. 


##Contributing

>Your contributions are welcome!

#####To do list
Some approaches to implement:
- TNR-MF [Yu et al., [ICWS'13](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6649559&tag=1)]
- TACF [Yu et al., [ICWS'14](http://ieeexplore.ieee.org/xpl/abstractAuthors.jsp?reload=true&arnumber=6928907)]

The goal of maintaining this repository is to make it simple to reproduce or modify the existing approaches. For future research on QoS prediction, you do not need to build your own program from scratch. The framework can be easily extended for new implementations.

>How to contribute page: [https://github.com/WS-DREAM/WSRec/wiki/How-to-contribute](https://github.com/WS-DREAM/WSRec/wiki/How-to-contribute)

Your implementations built on our framework are welcome to be hosted on this repository, or a external link can be directed to your own Github repository. Please feel free to contact us. Any contribution will be appreciated.


##### Contributors
A big thanks to WSRec contributors:
- [Jieming Zhu](http://jiemingzhu.github.io/), The Chinese University of Hong Kong (Project Founder)
- [Zibin Zheng](http://www.zibinzheng.com/), The Chinese University of Hong Kong (Contribute UIPCC)
- Pinjia He, The Chinese University of Hong Kong (Contribute HMF)
- [Yuwen Xiong](https://github.com/Orpine), Zhejiang University (Contribute TF, NTF, WSPred, OPred, Biased_MF, SVD++)
- Yifei Lu, Zhejiang University (Contribute ADF, T-WSRec)


##Subscribing to WS-DREAM Mail List
Subscribe to WS-DREAM mail list (wsdream.maillist@gmail.com) by filling out [the registration form](http://goo.gl/forms/gYR00pABub). You will receive messages about the update of the datasets and code release at the first moment. 


##Bugs, Discussion, Feedback
For bugs and discussions, please post to our issue page (https://github.com/WS-DREAM/WSRec/issues). For any other enquires, please contact [Mr. Jieming Zhu](http://jiemingzhu.github.io/).

##Copyright &copy;
Permission is granted for anyone to copy, use, modify, or distribute this program and accompanying programs and documents for any purpose, provided this copyright notice is retained and prominently displayed, along with a 
note saying that the original programs are available from our web page (https://wsdream.github.io/WSRec). The program is provided as-is, and there are no guarantees that it fits your purposes or that it is bug-free. All use of these programs is entirely at the user's own risk.

