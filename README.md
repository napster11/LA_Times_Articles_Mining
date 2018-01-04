# LA_Times_Articles_Mining

Objective
The purpose of this project is to write clustering algorithms based on k-means for clustering objects corresponding to 
sparse high dimensional vectors. The project consists of multiple components that involve getting the dataset, selecting 
the subset to cluster, pre-processing the dataset to convert it into a sparse representation, clustering the dataset, 
and evaluating the quality of the clustering solution. 

Getting the Dataset
The dataset for this assignment will be derived from the "Reuters-21578 Text Categorization Collection Data Set" 
that is available at the UCI Machine Learning Repository (link). Download the reuters21578.tar.gz file from the 
"Data Folder" (the file is 7.8MB compressed). Also, read the html file in the above folder to get an idea as to 
what the dataset is all about.



Selecting the Subset of the Dataset for Clustering
Process the various SGML files (.sgm) extension and select the articles that contain only a single topic. The topic is 
included within a pair of <TOPICS><D>topic</D></TOPICS> tags. Ignore the articles that do not contain any topic or 
have more than a single topic. A back of the envelope calculation shows that are about 9494 such articles. 
From these articles retain only the articles that correspond to topics that occur in the 20 most frequent topics. 

From the above set of articles, extract the NEWID number (in the <REUTERS> tag attribute), the single topic, and 
the text that is included within the <BODY>...</BODY> tags.  The body of the article will form the text that you 
will be using for clustering, the topic will act as a class label for evaluation, and the NEWID will be used as 
the ID of the article. 



Obtaining the Sparse Representation
Once you have extracted the text for each article, you need to derive a bag of words representation. In order to do 
that, you need to first clean up the text. To do that perform the following steps in that sequence:

1. Eliminate any non-ascii characters.
2. Change the character case to lower-case.
3. Replace any non alphanumeric characters with space.
4. Split the text into tokens, using space as the delimiter.
5. Eliminate any tokens that contain only digits.
6. Eliminate any tokens from the stop list that is provided (file stoplist.txt).
7. Obtain the stem of each token using Porter's stemming algorithm; you can use any of the implementations provided 
   here: https://tartarus.org/martin/PorterStemmer/.
8. Eliminate any tokens that occur less than 5 times.


Vector Representations
Collect all the tokens that remained after step 8 (above) across all articles and use them to represent each article 
as a vector in the distinct token space. For each document, derive three different representations by using the 
following approaches to assign a value to each of the document's vector dimension that corresponds to a token t 
that it contains:

The value will be the actual number of times that t occurs in the document (frequency).
The value will be 1+sqrt(frequency) for those terms that have a non-zero frequency.
The value will be 1+log2(frequency) for those terms that have a non-zero frequency.
The above three representations will be referred to as vector models in the rest of this document.
Once you have obtained those vectors, normalize them so that they are of unit length. These unit-length vectors will 
be the input to the clustering algorithms.


Here is a sample command line for your program:

kcluster input-file criterion-function class-file #clusters #trials output-file
The format of the input file must be in the (i,j,v) format, where i is the article ID (i.e., the NEWID), j is the dimension #, and v is the corresponding value (based on the vector model used). Note that the input file should be a comma-separated file. 

The format of the class file must be in the (i, label) comma-separated format, where i is the article ID and label is the topic.

The format of the output file must be in the (i, cluster#) comma-separated format, where i is the article ID and cluster# is a number between 0 and #clusters-1 indicating the cluster in which article i belongs to.

In your experiments, you should set #trials to be 20. Please use these 20 seeds: [1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39].

Upon finishing, your program should print the value of the criterion function for the best trial in addition to both the entropy and purity of the best clustering solution to the standard output.
