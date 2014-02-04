This software UIPCC is
developed by Zibin Zheng<zbzheng@cse.cuhk.edu.hk>, 2011
revised by Jieming Zhu<jmzhu@cse.cuhk.edu.hk>, 2014

Reference paper:
[ICWS'09] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, "WSRec: A Collaborative Filtering 
Based Web Service Recommender System", IEEE International Conference on Web Services, pp.437-444, 2009.
[TSC'11] Zibin Zheng, Hao Ma, Michael R. Lyu, and Irwin King, ¡°QoS-Aware Web Service Recommendation 
by Collaborative Filtering¡±, IEEE Transactions on Service Computing , vol.4, no.2, pp.140-152, 2011.


Five QoS prediction approaches are implemented in this code:
UMEAN
IMEAN
UPCC
IPCC
UIPCC(a.k.a WSRec) 

The source code is provided under "src" folder. You can generate the runnable *.jar package, 
and then execute the package using the provided python scripts.

Note: 
"resultFolder/" contains the results with five metrics: MAE, NMAE, RMSE, MRE, NPRE