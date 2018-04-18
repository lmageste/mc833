import numpy as np
import scipy as sp
import scipy.stats
import re

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return m, h, m-h, m+h

files = ["results/content_course.txt", "results/content_all_courses.txt", "results/detail_course.txt", "results/list_course.txt", "results/list_all_courses.txt", "results/remark_course.txt", "results/write_remark.txt"]

for file in files:
	totalExecution = []
	processing = []

	with open(file, "r") as f:
		for line in f:
			if "Time taken" in line:
				totalExecution.append(int(re.search(r'\d+', line).group()))
			elif "processing" in line:
				processing.append(int(re.search(r'\d+', line).group()))

	print totalExecution
	print processing

	print 'File: ' + file

	mean, stddev, minRange, maxRange = mean_confidence_interval(totalExecution)
	print 'Mean total execution time: ' + str(mean)
	print 'Standard deviation: ' + str(round(stddev, 3))
	print '95 pct confidence interval: ' + str(round(minRange, 3)) + ' to ' + str(round(maxRange, 3))

	mean, stddev, minRange, maxRange = mean_confidence_interval(processing)
	print 'Mean function processing time: ' + str(mean)
	print 'Standard deviation: ' + str(round(stddev, 3))
	print '95 pct confidence interval: ' + str(round(minRange, 3)) + ' to ' + str(round(maxRange, 3))

