----------------------------------------------------------------------------
README file for 339 * 5825 Web service QoS dataset
----------------------------------------------------------------------------

This dataset describes real-world QoS evaluation results from 339 users on 
5,825 Web services. Note that we have recently updated the location 
information (e.g., IP, AS, Latitude, Longitude) of users and services into 
the dataset. It is available for downloading at: 
http://www.cse.cuhk.edu.hk/~jmzhu/dataset/wsrec_dataset1.zip

----------------------------------------------------------------------------
List of contents of the dataset
----------------------------------------------------------------------------

userlist.txt  - information of 339 service users. Format: | User ID |  
                IP Address | Country | Continent | AS | Latitude | Longitude |
                Region | City |
wslist.txt    - information of the 5,825 Web services. Format: | Service ID |  
                WSDL Address | Service Provider | IP Address | Country | 
                Continent | AS | Latitude | Longitude | Region | City |
rtMatrix.txt  - 339 * 5825 user-item matrix of response-time. 
tpMatrix.txt  - 339 * 5825 user-item matrix for throughput.
readme.txt    - descriptions of the dataset. 

----------------------------------------------------------------------------
Reference paper
----------------------------------------------------------------------------

Please refer to the following papers for the detailed descriptions of this 
dataset:

- Zibin Zheng, Yilei Zhang, and Michael R. Lyu, “Investigating QoS of Real-
  World Web Services”, IEEE Transactions on Services Computing , vol.7, no.1, 
  pp.32-39, 2014.
  
- Zibin Zheng, Yilei Zhang, and Michael R. Lyu, “Distributed QoS Evaluation 
  for Real-World Web Services,” in Proc. of the 8th International Conference 
  on Web Services (ICWS'10), Miami, Florida, USA, July 5-10, 2010, pp.83-90.

IF YOU USE THIS DATASET IN PUBLISHED RESEARCH, PLEASE CITE EITHER OF THE ABOVE 
PAPERS. THANKS!

----------------------------------------------------------------------------
Acknowledgements
----------------------------------------------------------------------------

We would like to thank PlanetLab (http://www.planet-lab.org/) for collecting 
the dataset, and IPLocation (http://www.iplocation.net/) for collecting the 
location information. We also thank Prof. Mingdong Tang (HNUST) for 
contributing the AS information of the users and services. 

----------------------------------------------------------------------------
Copyright
----------------------------------------------------------------------------

Our datasets are freely available for research purposes. Downloading and 
using the dataset will indicate your acceptance to enter into a GNU General 
Public License agreement. Redistribution of this dataset to any other third 
party or over the Web is not permitted.

