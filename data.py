# -*- coding: utf-8 -*-
import re
import math
from collections import OrderedDict
from collections import Counter
from porter import PorterStemmer

porterStemmer = PorterStemmer()

articleStemMap=OrderedDict()                        #Article to it's words map
idTopicMap=OrderedDict()                            #NEW_ID To Topic Map
stopList={}                                         #Store the words of stoplist file
wordFreq=OrderedDict()                              #Use Word frequency to remove the words with frequency < 5 later
topicFreq=OrderedDict()                             #Capture Topics frequency to remove some non popular articles
articleWordCount=OrderedDict()                      #Total No. of words in an article

def filterNonAsciiChar(word):
    return ''.join(i for i in word if ord(i)<128)

def parseBody(body, ID,topic):

    # print(ID)
    # print(topic)
    wordList = OrderedDict()                                 #Store the words of a particular NEW_ID
    idTopicMap[ID] = topic
    if len(body) > 0:
        body = body[0].splitlines()
    else:
        return


    if topic not in topicFreq:
        topicFreq[topic]=1
    else:
        topicFreq[topic] +=1

    for line in body:
        line = filterNonAsciiChar(line).lower()      #Filter Non Ascii Characters and change to lower case
        line = re.sub('[^0-9a-zA-Z]+', ' ', line)    #Replace Non Alphanumeric with space
        line = line.strip()
        for word in line.split(' '):
            word = word.strip()

            # Discard space and token with digits only
            if not word.isspace() and not word.isdigit() and word not in stopList:
                word = porterStemmer.stem(word,0,len(word)-1)

                if word not in wordFreq:
                    wordFreq[word] = 1
                else:
                    wordFreq[word] += 1

                #Word count in a document
                if word not in wordList:
                    wordList[word] = 1
                else:
                    wordList[word] += 1

    #Save the map of ID and Term Vector
    articleStemMap[ID] = wordList
    
def parse(TextData):
    #print(TextData)
    reuters = re.findall('<REUTERS.*?>', TextData)
    body = re.findall('<BODY.*?>(.*)</BODY>', TextData, re.MULTILINE | re.DOTALL)
    #print(reuters)
    #print(topics)
    #print(body)

    topicYesFlag = False
    NEW_ID = -1

    if len(reuters) > 0:
        tags = reuters[0].split(' ')                        #Split the <REUTERS> tag to fetch if <TOPICS=YES
        for st in tags:
            keyValues = st.split('=')
            if keyValues[0] == 'TOPICS':
                if keyValues[1] == '"YES"':                 #Check if TOPICS Tag is Yes or not
                    topicYesFlag = True
                else:
                    topicYesFlag = False
                
            if keyValues[0] == 'NEWID':                     #Find NEWID value if TOPICS is yes
                NEW_ID = keyValues[1].split('>')[0][1:-1]   #Trim first and last double quotes

        if topicYesFlag  and NEW_ID != -1:
            topics = re.findall('<TOPICS.*?><D>(.*?)</D></TOPICS>',TextData)
            if len(topics) > 0:
                topics = topics[0].split('</D>')
                if len(topics) == 1:
                    parseBody(body, NEW_ID, topics[0])


def parseStopList(content):

    stopWords = content.split(' ')
    for stop in stopWords:
        if stop not in stopList:
            stopList[stop] = 1

def main():
    fstoplist = open("stoplist.txt","r")                #Store the stoplist words in a dictionary
    stopWords = fstoplist.read()
    parseStopList(stopWords)

    n=22
    i = 0
    fileName="reuters21578/reut2-0"
    while i < n:
        name=fileName
        if i < 10:
            name += "0"+`i`+".sgm"
        else:
            name += `i`+".sgm"

        f = open(name, "r")
        content = f.read()

        result = re.findall('<REUTERS.*?>(.*)</REUTERS>', content, re.MULTILINE | re.DOTALL)
        if len(result) > 0:
            result = result[0].split('</REUTERS>')

        temp=content.splitlines()[1]
        reuterEmptyCheck=0
        for  x in result:
            if reuterEmptyCheck == 0:
                x=temp+x
                #print(x)
                reuterEmptyCheck = reuterEmptyCheck+1
            parse(x)                                        #Parse the words of BODY tags and store them in a hash with NEW_ID as key and side by side maintain hash of ID and topics

        i = i+1


    #Find Frequent Topics
    freqTopicList=[]
    counter=Counter(topicFreq).most_common(20)
    for k,v in counter:
        freqTopicList.append(k)

    #filter Data based on word Count and topic frequency
    cnt=0
    cnt2=0
    cnt4=0

    classFile = open("reuters21578.class", "w")

    for ID, valueHash in articleStemMap.items():
        wordCount=0
        if idTopicMap[ID] not in freqTopicList:                 #Delete articles with topic which is not in frequent 20 Topics list
            del articleStemMap[ID]
            continue

        finalWordList={}                                        #Remove words from an article with freq less than 5
        for key,value in articleStemMap[ID].items():            #Delete all words of freq < 5 from a Article with topic freq >= 20
            if(wordFreq[key] >= 5 and len(key) > 0):
                finalWordList[key]=value
                wordCount+=value

        articleStemMap[ID] = finalWordList
        articleWordCount[ID]=wordCount
        entry = ID + ',' + idTopicMap[ID] + "\n"  # Write (ID,TOPIC) To Class File
        classFile.write(entry)

        if len(articleStemMap[ID]) > 0:
            #print(ID)
            #print(idTopicMap[ID])
            #print(wordCountMap[ID])

            cnt = cnt+1

    classFile.close()
        #print((finalWords))

    #print(articleStemMap['11574'])
    #print(articleWordCount['11574'])
    #print(cnt)
    dimensionColumn=''
    for key,val in wordFreq.items():                            #Delete Words with Freq less than 5
        if(val >= 5 and len(key) > 0):
            dimensionColumn+= key+','
            continue
        else:
            del wordFreq[key]
    #print(len(wordFreq))
    #print(list(wordFreq.keys()).index("talyzin"))
    #print(wordFreq)


    # stemKeyIndex = OrderedDict()
    # ind = 1
    # for wordKey, val in wordFreq.items():
    #     stemKeyIndex[wordKey] = ind
    #     #inp=wordKey+' , '+`ind`+"\n"
    #     #inputFile.write(inp)
    #     ind = ind + 1


    heading="NEW_ID,   Dimension#,  Value"+"\n"
    inputFile1 = open("freq.csv", "w")
    inputFile2 = open("sqrtfreq.csv","w")
    inputFile3 = open("log2freq.csv","w")
    inputFile1.write(heading)
    inputFile2.write(heading)
    inputFile3.write(heading)
    tokenLabelFile = open("reuters21578.clabel","w")
    
    print(len(articleStemMap))
    print(len(wordFreq))
    
    tokenFlag=0
    for ID,StemList in articleStemMap.items():
        index=1
        for key,value in wordFreq.items():
            if tokenFlag==0:
                token = key+"\n"
                tokenLabelFile.write(token)

            if key in StemList:
                line3 = ID+','+`index`+','+`1+math.log(float(StemList[key]),2)`+"\n"
               #line2 = ID+','+`index`+','+`1+math.sqrt(float(StemList[key])`+"\n"
                line1 = ID+ ',' + `index` + ',' + `StemList[key]` + "\n"
                line2 = ID+','+`index`+','+`1+math.sqrt(float(StemList[key]))`+"\n"
                inputFile1.write(line1)
                inputFile2.write(line2)
                inputFile3.write(line3)
            else:
                line = ID +','+ `index` + ','+ `0` + "\n"
                inputFile1.write(line)
                inputFile2.write(line)
                inputFile3.write(line)
            index=index+1
        tokenFlag=1
    inputFile1.close()
    inputFile2.close()
    inputFile3.close()
    tokenLabelFile.close()


if __name__ == "__main__": main()
