import numpy as np
import scipy as sp
import scipy.stats
import re

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = round(np.mean(a)), scipy.stats.sem(a)
    h = round(se * sp.stats.t._ppf((1+confidence)/2., n-1), 0)
    return m, h, m-h, m+h

files = ["results_udp/content_course.txt", "results_udp/content_all_courses.txt", "results_udp/detail_course.txt", "results_udp/list_course.txt", "results_udp/list_all_courses.txt", "results_udp/remark_course.txt", "results_udp/write_remark.txt"]

for file in files:
	totalExecution = []
	processing = []

	with open(file, "r") as f:
		for line in f:
			if "time taken" in line:
				totalExecution.append(int(re.search(r'\d+', line).group()))
			elif "processing" in line:
				processing.append(int(re.search(r'\d+', line).group()))

	print totalExecution
	print processing

	print 'File: ' + file

	mean, stddev, minRange, maxRange = mean_confidence_interval(totalExecution)
	print 'Mean total execution time: ' + str(mean)
	print 'Standard deviation: ' + str(stddev)
	print '95 pct confidence interval: ' + str(minRange) + ' to ' + str(maxRange)

	mean, stddev, minRange, maxRange = mean_confidence_interval(processing)
	print 'Mean function processing time: ' + str(mean)
	print 'Standard deviation: ' + str(stddev)
	print '95 pct confidence interval: ' + str(minRange) + ' to ' + str(maxRange)

