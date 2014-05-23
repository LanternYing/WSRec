#########################################################
# IP2Geo_crawler.py
# Author: Pinjia He <PunyTitan@GitHub> 
# Revised: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/23
# Crawl the location information of users and services
# by IP2Geo service (http://www.iplocation.net/)
#########################################################


# external package: beautifulsoup4-4.3.2
from bs4 import BeautifulSoup
import urllib2
import numpy as np
import time
import sys


#########################################################
def main():
	# proxy set up
	proxy = urllib2.ProxyHandler({'http': "proxy.****:8000"})
	 
	# create an URL opener using proxy
	opener = urllib2.build_opener(proxy)
	urllib2.install_opener(opener)

	#read files information
	userlist = '../data/dataset#1/userlist.txt'
	wslist = '../data/dataset#1/wslist.txt'
	data = np.genfromtxt(userlist, dtype=np.str, comments='$', 
		delimiter='\t', skip_header=2)
	userIP = data[:, 0:2]
	userAS = data[:, 4]

	data = np.genfromtxt(wslist, dtype=np.str, comments='$', 
		delimiter='\t', skip_header=2)
	wsIP = data[:, 0:4]
	wsAS = data[:, 6]


	saveUserFile = open("userlist.txt", "a")
	saveWsFile = open("wslist.txt", "a")

	listwaiting = []
	last_ip = ""

	for i in range(userIP.shape[0]):
		uid = userIP[i, 0]
		ip = userIP[i, 1]
		(country, region, city, continent, latitude, longitude) = ip2geo(ip)
		line = uid + '\t' + ip + '\t' + country + '\t' + continent + '\t'\
			+ userAS[i] + '\t' + latitude + '\t' + longitude + '\t' +\
			region + '\t' + city + '\n'
		saveUserFile.write(line)
		print line

	saveUserFile.close()

	for i in range(wsIP.shape[0]):
		wid = wsIP[i, 0]
		ip = wsIP[i, 3]
		(country, region, city, continent, latitude, longitude) = ip2geo(ip)
		line = wid + '\t' + wsIP[i, 1] + '\t' + wsIP[i, 2] + ip + '\t' + country +\
			'\t' + continent + '\t' + wsAS[i] + '\t' + latitude + '\t' + longitude +\
			'\t' + region + '\t' + city + '\n'
		saveWsFile.write(line)
		print line

	saveWsFile.close()
#########################################################


#########################################################
def ip2geo(ip):
	if ip == 'null':
		return ('null', 'null', 'null', 'null', 'null', 'null')

	url = "http://www.iplocation.net/?query=" + ip
	page = urllib2.urlopen(url)
	soup = BeautifulSoup(page)
	# parse the page by using contents attribute
	try:
		target1 = soup.contents[19].contents[1].contents[3].contents[3].contents[1]\
					.contents[0].contents[1].contents[3].contents[0].contents[1]\
					.contents[1].contents[1].contents[0].contents[21]
		# take the data from table: Geolocation data from IPligence (Product: Max)
		country = str(target1.contents[3].contents[1].string)
		region = str(target1.contents[3].contents[2].string)
		city = str(target1.contents[3].contents[3].string)
		# recover from missing data
		if region == 'None':
			target2 = soup.contents[19].contents[1].contents[3].contents[3].contents[1]\
					.contents[0].contents[1].contents[3].contents[0].contents[1]\
					.contents[1].contents[1].contents[0].contents[16]
			region = str(target2.contents[3].contents[2].string)
			city = str(target2.contents[3].contents[3].string)
		continent = str(target1.contents[7].contents[1].string)
		latitude = str(target1.contents[7].contents[2].string)
		longitude = str(target1.contents[7].contents[3].string)
	except IndexError:
		time.sleep(20)
		print 'Exception! ip: ', ip
		sys.exit()

	return (country, region, city, continent, latitude, longitude)
#########################################################


if __name__ == "__main__": main()
