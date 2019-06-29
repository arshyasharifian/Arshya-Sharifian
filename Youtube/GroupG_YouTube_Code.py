# -*- coding: utf-8 -*-
"""
Created on Sat Mar  2 19:17:49 2019

@author: mwoo01
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans

from collections import Counter
from nltk.corpus import stopwords
import nltk
nltk.download('stopwords')

# for ML application
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import r2_score
from sklearn import metrics
import seaborn as sns
from sklearn.metrics import mean_squared_error

# Read in data file
dir = 'C:/Users/mwoo01/Desktop/Personal/DataScience/Project/'
df_raw = pd.read_csv(dir + 'USvideos.csv')
df = df_raw[df_raw.video_error_or_removed == 0]
df = df.drop_duplicates()

#%%
# *********************************Popular Tags ****************************************************************
# initialize lists
tag_list = []
all_tags = []

# Iterate through dataframe collect all tags
for index,row in df.iterrows():
    #tag_cnt = str(row['tags']).count('|') + 1
    tag_list = str(row['tags']).split('|')
    all_tags.extend(tag_list)

# limit to top 20 tags    
counts = Counter(all_tags).most_common(20)

# Export tags to excel for use in Tableau
df_tags = pd.DataFrame(all_tags)
df_tags.to_excel('C:/Users/mwoo01/Desktop/Personal/DataScience/Project/all_tags.xlsx')

# Plot the top 20 tags in a bar chart
plt.bar(*zip(*counts), color='c')
plt.title('Most Popular Tags', fontsize=18)
plt.ylabel("Frequency")
plt.xlabel("Tag")
plt.xticks(rotation='vertical')
plt.show()

#%%
# *************************Likability vs Engagement ****************************************************
# How likeable is the video?
like_index = df['likes']/df['dislikes']
# How engaged are viewers of this video
engagement_index = df['comment_count']/df['views']

# Plot to see relationship between likeability and engagement
plt.scatter(like_index,engagement_index,color='c', edgecolors='black', linewidths=0.25)
plt.title('Likable vs Engagement', fontsize=18)
plt.ylabel("Engagement Index")
plt.xlabel("Like Index")
plt.show()

#%%
# ****************************Top categories **************************************************************
df1 = df['category_id']

vid_cat = {}
for key in df1:
    if key in vid_cat:
        vid_cat[key] += 1
    else:
        vid_cat[key] = 1

for keys, values in vid_cat.items():
    print (keys, values)

df1.value_counts().plot(kind = 'bar')
plt.ylabel('Number of Times Highest Trending')
plt.xlabel('Category')
plt.title('Count of Highest Trending Category', fontsize=18)
plt.show()

#%%
# ***************************Likes vs dislikes clusturing ************************************************
#start classification
f1 = df['dislikes']
f2 = df['likes']

X = np.array(list(zip(f1, f2)))

k = 3
kmeans = KMeans(n_clusters = k)
kmeans = kmeans.fit(X)
labels = kmeans.predict(X)
centroids = kmeans.cluster_centers_

fig2 = plt.figure()
#111 defines the type of grid: 1X1
kx = fig2.add_subplot(111)

for i in range(k):
    #I don't know what this does, creates points in the map I am guessing
    points = np.array([X[j] for j in range(len(X)) if labels[j] == i])
    #this places the points in the scatter plot
    kx.scatter (points[:, 0], points[:, 1], s=20, cmap = 'rainbow')

#defines the star, the centroid on the scatter plot
kx.scatter(centroids[:, 0], centroids[:, 1], marker = '*', s=200, c='#050505')
print("final centroids")
print(centroids)

plt.ylabel('Number of Likes')
plt.xlabel('Number of Dislikes')
plt.title('Likes vs. Dislikes for Highest Trending Videos', fontsize=18)

plt.show()

#%%
# ************************Title words and length analysis *********************************************
#remove non alphabetic characters from the USvideos_data['title']
non_alphabetic = df['title'].replace('[^a-zA-Z0-9 ]', '', regex=True)

#non_alphabetic is a series in panda, convert this to a list
non_alphabetic_list = non_alphabetic.tolist()

#split list of strings to single elements
newlist = [word for line in non_alphabetic_list for word in line.split()]

#remove stop words from newlist, regardless of case sensitivity
stop = stopwords.words('english')
filtered_words = [word for word in newlist if word.lower() not in stop]

#removing digits in list of strings
final_list = [x for x in filtered_words if not (x.isdigit() 
                                         or x[0] == '-' and x[1:].isdigit())]

#removing case sensitivity in final_list
final_list2 = [(x.lower()) for x in final_list]

#find common words in titles; map common keywords to a counter
common = Counter(final_list2).most_common(25)

#print the common list
print(common)
#space
print()
print ('There is a total of', len(final_list2), 'title keywords found in the Youtube data set')

#visualization for keywords found in video titles
Keywords_figure = plt.bar(*zip(*common), color ='red')
plt.title('Popular Keywords found in Youtube Titles', fontsize=18)
plt.ylabel("Counts", fontsize=12)
plt.xlabel("Keyword", fontsize=12)
plt.xticks(rotation='vertical')
plt.show()

#plt.savefig('Keywords_figure.jpg')

#counting number of characters in each youtube video title
df["title_length"] = df["title"].apply(lambda a: len(a))
        
#plot title length vs. view count
View_count = df['views']
Title_length = df['title_length']
fig, plot =plt.subplots()
plt.scatter(Title_length, View_count, color= 'red', edgecolors='grey', linewidths=0.25)
plot.set(xlabel="Title Length (# of characters)", ylabel="View Count", title='Title Length vs. View Count', xticks=range(0, 110, 10))
plt.savefig('titlelength.jpg')

#%%
############## FIRST ML PASS (Predict views based on like count#########################################)
#retreiving column 8 (likes) and column 7 (views) and converting it to a array/reshaping the array to a single feature
X = (df.iloc[:,8].values).reshape(-1, 1)
y = (df.iloc[:,7].values).reshape(-1, 1)


lm = LinearRegression()
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.30)
lm.fit(X_train, y_train)

prediction = lm.predict(X_test)
plt.scatter(X_test, y_test,  color='purple', edgecolors='grey', linewidths=0.25)
plt.plot(X_test, prediction, color='black', linewidth=3)
plt.xlabel("Likes")
plt.ylabel("Views")
plt.title('Predicted Views vs. Actual Views')

print("R-squared is a statistical measure of how close the data are to the fitted regression line.")
print ()
print ("In our model, the R^2 score is: ", r2_score(y_test, prediction))
print ()

print("In statistics, mean absolute error (MAE) is a measure of difference between two continuous variables.")
print ()
print("In our model, the mean absolute error is: ", metrics.mean_absolute_error(y_test, prediction))
print()

print("In statistics, the mean squared error (MSE) or mean squared deviation (MSD) of an estimator (of a procedure for estimating an unobserved quantity) measures the average of the squares of the errors—that is, the average squared difference between the estimated values and what is estimated.")
print ()
print("In our model, the mean squared error is: ", metrics.mean_squared_error(y_test, prediction))
print()

print("In our model, the root mean squared error is: ", np.sqrt(metrics.mean_squared_error(y_test, prediction)))
print ()



#%%

########### SECOND PASS AT ML (Predict views based on likes and category)###############################
#one hot encoding for categorical values
categorical_val = pd.get_dummies(df['category_id'])

#converting to numpy arrays for easier use
categorical_val = np.asarray(categorical_val)
data_converted = np.asarray(df['likes'])

#reshaping to combine arrays
data_converted = data_converted.reshape(40878, 1)

#combining arrays
combo_data = np.concatenate((categorical_val, data_converted), axis=1)

X = combo_data
y = df['views']

lm = LinearRegression()
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.10)
lm.fit(X_train, y_train)

prediction = lm.predict(X_test)
'''
R^2 (coefficient of determination) regression score function.
Best possible score is 1.0 and it can be negative (because the model can be arbitrarily worse). 
A constant model that always predicts the expected value of y, disregarding the input features, would get a R^2 score of 0.0.
source: https://scikit-learn.org/stable/modules/generated/sklearn.metrics.r2_score.html
'''
print ("R^2 score is: ", r2_score(y_test, prediction))
'''
we receive a high means squared score likely due to some outlier.  If we had more time, we would remove outliers.
the mean squared score squares sums the difference between the test and prediction.  Here, a number close to 0 is best,
but likely due to outliers in the dataset, we have a very high means squared score.
source: https://en.wikipedia.org/wiki/Mean_squared_error
'''
print ("Means squared score is: ", mean_squared_error(y_test, prediction))


    

    