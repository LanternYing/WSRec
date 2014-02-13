This source code in "PMF.py/" directory is developed by Jieming Zhu<jmzhu@cse.cuhk.edu.hk>, 2014/2/6.

Probabilistic Matrix Factorization (PMF) approach is implemented with Python.

The source code is provided under "src" folder. You can directly run the provided Python scripts "run_rt.py" and "run_tp.py". It is worth noting that our implementation is based on numpy, scipy, and matplotlib packages, so you need to first install these packages (http://www.scipy.org/).

Note: 
"result/" contains the experimental results with five metrics (MAE, NMAE, RMSE, MRE, NPRE), after running the above five QoS prediction approaches on "data/dataset#1". E.g., "result/rtResult_0.20.txt" records the evaluation results under matrix density = 20%.

Reference paper:
[1] Zibin Zheng, Hao Ma, M.R. Lyu, and Irwin King, "Collaborative Web Service QoS Prediction via Neighbourhood Integrated Matrix Factorization," IEEE Transactions on Service Computing (TSC), vol. 6, no. 3, 2013, pp. 289-299. 
 
[2] R. Salakhutdinov and A. Mnih, "Probabilistic Matrix Factorization," Proc. Advances in Neural Information Processing Systems, pp. 1257-1264, 2007.

