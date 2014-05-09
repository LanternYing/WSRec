##WSRec: Web Service Recommendation

This repository contains the implementations on most of the state-of-the-art approaches for Web service QoS Prediction, including:
	UMEAN
	IMEAN
	UPCC
	IPCC
	UIPCC
	PMF
	
Most of the algorithms are implemented with Python. It is worth noting that our implementation is based on numpy, scipy, and matplotlib packages, so you need to first install these packages ([http://www.scipy.org/](http://www.scipy.org/)). 

Meanwhile, to speed up, we make use of [Cython](http://cython.org/ "Cython's Web page") (a language to write C extensions for Python) to implement the core functions. As a consequence, Cython needs to be installed as an external package. In our repository, we download the package [here](https://github.com/cython/cython/archive/master.zip "The latest release of Cython"), and put it into the "external/" folder.

You need to install Cython as follows:
```
vi ~/.bash_profile
export PYTHONPATH="$PYTHONPATH:/WSRec/external/Cython-0.20.1/"
export PATH="PATH:/WSRec/external/Cython-0.20.1/bin/"
```

###How to Contribute


