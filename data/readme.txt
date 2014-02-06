This data folder provides two datasets:

dataset#1:
------------------------------------------------------
339 * 5825 Web service QoS dataset
You can download the entire dataset at: http://www.cse.cuhk.edu.hk/~jmzhu/dataset/wsrec_dataset1.zip

This dataset describes real-world QoS evaluation results from 339 users on 5,825 Web services, which includes five files:

"readme.txt": descriptions of the dataset.

"userlist.txt": information of 339 service users. 
	Format: | User ID | IP address of user | country | longitude | latitude |
	
"wslist.txt": information of the 5,825 Web services. 
	Format: | WS ID | WSDL address | provider name | country name |
	
"rtMatrix.txt": 339 * 5825 user-item matrix of response-time. 

"tpMatrix.txt": 339 * 5825 user-item matrix for throughput. 

Reference paper:
Zibin Zheng, Yilei Zhang, and Michael R. Lyu, “Distributed QoS Evaluation for Real-World Web Services,” in Proc. of the 8th International Conference on Web Services (ICWS2010), USA, 2010, pp.83-90.



dataset#2:
------------------------------------------------------
142 * 4500 * 64 time-aware Web service QoS dataset
You can download the entire dataset at: http://www.cse.cuhk.edu.hk/~jmzhu/dataset/wsrec_dataset2.zip

This dataset describes real-world QoS evaluation results from 142 users on 4,500 Web services over 64 different time slices, which includes three files:

"rtData" (492 MB): response-time values of 4,500 Web services when invoked by 142 service users over 64 time slices. The data format is as follows:
User ID | Service ID | Time Slice ID | Response Time (sec)
e.g.: 98    4352    33    0.311

"tpData" (585 MB): throughput values of 4,500 Web services when invoked by 142 service users in 64 time slices. The data format is as follows:
User ID | Service ID | Time Slice ID | Throughput (kbps)
e.g.: 91    1196    62    32.88

"readme.txt" (1 KB): descriptions of the dataset.

Reference paper:
Yilei Zhang, Zibin Zheng, and Michael R. Lyu, "WSPred: A Time-Aware Personalized QoS Prediction Framework for Web Services", in Proceedings of the 22th IEEE Symposium on Software Reliability Engineering (ISSRE 2011).


