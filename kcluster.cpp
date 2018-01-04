#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<vector>
#include<algorithm>
#include<cstring>
#include<limits>
#include<cfloat>
#include<cmath>
#include<cmath>

using namespace std;

string outPutFileName;
int criterionFunctionFlag;
string testFile;
string qualityFileName;
ofstream out;
ofstream labelFile;
int trials;
double vectorLength(vector<double> v) {
    int n = v.size();
    double r = 0.0;

    for(int i = 0;i < n;i++) {
        r += v[i]*v[i];
    }
    return sqrt(r);
}
double dist(vector<double> x, vector<double> cent) {

    double dis = 0.0;

    if(criterionFunctionFlag == 1) {
        int n = min(x.size(),cent.size());
        for(int i = 0;i < n;i++) {
            dis += pow((x[i]-cent[i]),2);
        }
        return sqrt(dis);
    }else if(criterionFunctionFlag == 2 || criterionFunctionFlag == 3) {
        int n = min(x.size(),cent.size());
        for(int i = 0;i < n;i++) {
            dis += x[i]*cent[i];
        }
        return (dis/(vectorLength(x)*vectorLength(cent)));
    }else {
        return dis;
    }
}
double calculateError(vector<vector<double> > newCentroids, map<int, vector<string> >* clusterList, map<string,vector<double> >* vectorModel, int k) {
    double result = 0.0;
    if(criterionFunctionFlag == 1) {
        for(int i = 0;i < k;i++) {
            int m = (*clusterList)[i].size();
            for(int j = 0;j < m;j++) {
                string temp = (*clusterList)[i][j];
                result += pow(dist(newCentroids[i], (*vectorModel)[temp]),2);
            }
        }
    }else if(criterionFunctionFlag == 2) {
        for(int i = 0;i < k;i++) {
            int m = (*clusterList)[i].size();
            for(int j = 0;j < m;j++) {
                string temp = (*clusterList)[i][j];
                result += dist(newCentroids[i], (*vectorModel)[temp]);
            }
        }
    }
    else if(criterionFunctionFlag == 3) {
        int totalSize = 0;
        double groupedCentroid = 0.0;
        vector<double> globalCentroid;
        int flag = 0;
        for(int i = 0;i < k;i++) {
            int m = (*clusterList)[i].size();
            totalSize += m;
            for(int j = 0;j < m;j++) {
                string temp = (*clusterList)[i][j];
                if(flag == 0) {
                    globalCentroid = (*vectorModel)[temp];
                    flag = 1;
                }else {
                    vector<double> vec = (*vectorModel)[temp];
                    int vecSize = vec.size();
                    for(int p = 0;p < vecSize;p++) {
                        globalCentroid[p] += vec[p];
                    }
                }
            }
        }
        //out << " total no. of points " << totalSize << "\n";
        int len = globalCentroid.size();
        for(int t = 0;t < len;t++) {
            globalCentroid[t] /= totalSize;
        }

        for(int i = 0;i < k;i++) {
            result += newCentroids[i].size()*(dist(globalCentroid, newCentroids[i]));
        }
    }
    return result;
}


bool isConverged(vector<vector<double> > old, vector<vector<double> > updated, int k) {
    double threshold = 0.1;
    double cthreshold = 0.99;
    if(k > 20) {
        threshold = 0.12;
        cthreshold = 0.90;
    }
    for(int i = 0;i < k;i++) {
        double temp = dist(old[i],updated[i]);
        //out << "index " << i << " dist " << temp << endl; 
        if(criterionFunctionFlag == 1 && temp > threshold) {
            return false;
        }else if((criterionFunctionFlag == 2 || criterionFunctionFlag == 3) && temp < 0.90) {
            return false;
        }
    }
    return true;
}
vector<double> findCentroid(vector<string> v, map<string,vector<double> >* vectorModel) {

        clock_t findCentS,findCentE;
        findCentS = clock();
        vector<double> result;
        int n = v.size();
        vector<double> s = (*vectorModel)[v[0]];
        int m = s.size();
        for(int i = 0;i < m;i++) {
            for(int j = 1;j < n;j++) {
                s[i] += (*vectorModel)[v[j]][i];
            }
            s[i] /= n;
        }
        findCentE = clock();
        cout << "find centroid time " << ((double)(findCentE-findCentS))/CLOCKS_PER_SEC << endl;
        return s;

}
void convergence(vector<vector<double> > centroids, map<string,vector<double> >* vectorModel, int k, double* err, map<int, vector<string> >* clusterList) {

    map<string,vector<double> >::iterator it;
    map<int, vector<double> > temp;

    clock_t itS,itE,x,y;
    itS = clock();
    x = clock();
    for(it = vectorModel->begin();it != vectorModel->end();it++) {
        int clusterInd = -1;
        double mx;
        for(int j = 0;j < k;j++) {
            vector<double> c = centroids.at(j);
            if(j == 0) {
                mx = dist((*it).second, c);
                clusterInd = 0;
            }else {
                double distance = dist((*it).second, c);
                if(criterionFunctionFlag == 1 && distance < mx) {
                    mx = distance;
                    clusterInd = j;
                }else if((criterionFunctionFlag == 2 || criterionFunctionFlag == 3) && distance > mx) {
                    mx = distance;
                    clusterInd = j;
                }
            }
        }
        (*clusterList)[clusterInd].push_back((*it).first); 

        if(temp.find(clusterInd) == temp.end()) {
            temp.insert(pair<int,vector<double> >(clusterInd,(*it).second));
        }else {
            vector<double> c = temp[clusterInd];
            int l = c.size();
            for(int t = 0;t < l;t++) {
                c[t] += (*it).second.at(t);
            }
            temp[clusterInd] = c;
        }
    }
    y = clock();
    //out << "cluster Assignment time " << ((double)(y-x))/CLOCKS_PER_SEC << endl;
    //for(int i = 0;i < k;i++) {
    //    out << (*clusterList)[i].size() << endl;
        // if((*clusterList)[i].size() == 0) {

        // }
    //}

    //Check if any cluster is empty
    vector<vector<double> > newCentroids;
    for(int i = 0;i < k;i++) {
        int newCentSize = temp[i].size();
        for(int j = 0;j < newCentSize;j++) {
            temp[i][j] /= (*clusterList)[i].size();
        }
        newCentroids.push_back(temp[i]);
    }
    itE = clock();
    //out << "one iteration time " << ((double)(itE-itS))/CLOCKS_PER_SEC << endl;

    //out << "New Centroids Processed\n";
    if(isConverged(centroids, newCentroids,k)) {
        //out << "True\n";
        (*err) = calculateError(newCentroids, clusterList, vectorModel,k);
        return;
    }else {
        clusterList->clear();
        convergence(newCentroids,vectorModel,k, err,clusterList);
    }
}
void calculateEntropy(map<int,vector<string> > res, map<string,string>* classLabels, map<string,vector<double> >* vectorModel, int k) {
    map<int,vector<string> >::iterator rit;
    map<string,string>::iterator cit;
    map<string,int> labelCount;                 //Count the labels Count of original classLabels
    map<string,int>::iterator lCount, rlc;
    int n = (*classLabels).size();
    for(cit = (*classLabels).begin();cit != (*classLabels).end();cit++) {
        labelCount[(*cit).second]++;
    }
    string labelString = "";
    for(lCount = labelCount.begin();lCount != labelCount.end();lCount++) {
        labelString += (*lCount).first + ",";
    }
    labelFile << labelString << "\n";

    int totalItems = 0;
    for(int i = 0;i < res.size();i++) {
        totalItems += res[i].size();
    }

    vector<map<string,int> > entropyMat;
    double totalEntropy = 0.0;
    double totalPurity = 0.0;
    double MaxPurity = 0.0;
    int PurestCluster;
    for(int i = 0;i < res.size();i++) {
        map<string,int> clusterLabels;
        int m = res[i].size();
        //cout << "cluster " << i << " Size " << m;
        for(int j = 0;j < m;j++) {
            string classType = (*classLabels)[res[i][j]];
            //cout << "classType " << classType << endl;
            clusterLabels[classType]++;
        }
        double ansEntropy = 0.0;
        double ansPurity = 0.0;
        string countRes = "";
        for(rlc = labelCount.begin();rlc != labelCount.end();rlc++) {
            double p = 0.0;
            if(clusterLabels.find((*rlc).first) == clusterLabels.end()) {
                //out << " Not Found\n";
                countRes += "0,"; 
                ansEntropy += 0.0;
            }else {
                countRes += to_string(clusterLabels[(*rlc).first]) + ",";
                //out << " class Count " << (*rlc).first << " " << clusterLabels[(*rlc).first] << endl;
                if(m != 0) {
                    p = ((double)clusterLabels[(*rlc).first])/m;
                    //out << " prob " << p << endl;
                    ansEntropy += p*(log2(p));
                    //out << "ansEntropy " << ansEntropy << endl;
                }
                
            }
            if(p > ansPurity) {
                ansPurity = p;
            }
        }
        if(ansPurity > MaxPurity) {
            MaxPurity = ansPurity;
            PurestCluster = i;
        }
        labelFile << countRes << "\n";
        //out << " purity for cluster " << i << " " << ansPurity << "\n";
        double weight = ((double)m)/totalItems;
        totalEntropy += weight*ansEntropy;
        totalPurity += weight*ansPurity;
    }
    labelFile << "Final Entropy " << totalEntropy << "\n";
    labelFile << "Final Purity " << totalPurity << "\n";
    labelFile << "Cluster " << PurestCluster << " has largest purity " << MaxPurity << "\n";
}

//VectorModel should be changed into the Unit Length Vector
void kMeans(vector<string> ids, map<string,string>* classLabels, map<string,vector<double> >* vectorModel, int k, int trials) {


    int n = classLabels->size();
    int seed[20] = {1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39};
    double finalError;
    int opt;
    vector<map<int, vector<string> > > results;
    for(int i = 0;i < trials;i++) {
        srand(seed[i]);
        vector<vector<double> > centroids;

        for(int t = 0;t < k;t++) {
            int rn = rand()%n;
            vector<double> cent = (*vectorModel)[ids[rn]];
            centroids.push_back(cent);
        }
        double cError = 0.0;
        clock_t conS,conE;
        conS = clock();
        map<int, vector<string> > clusterList;
        convergence(centroids, vectorModel, k,&cError,&clusterList);
        results.push_back(clusterList);
        conE = clock();
        
        //out << " Convergence total time " << ((double)(conE-conS))/CLOCKS_PER_SEC << endl;
        //out << "error " << cError << endl;
        if(i == 0) {
            finalError = cError;
            opt = i;
        }else {
            if((criterionFunctionFlag == 1 || criterionFunctionFlag == 3) && (cError < finalError)) {
                finalError = cError;
                opt = i;
            }else if(criterionFunctionFlag == 2 && (cError > finalError)) {
                finalError = cError;
                opt = i;
            }
        }
    }

    //out << "final Error " << finalError << " " << opt << endl;
    map<int,vector<string> > res = results[opt];
    ofstream result(outPutFileName);
    for(int i = 0;i < k;i++) {
        int n = res[i].size();
        //out << " size " << n << endl;
        for(int j = 0;j < n;j++) {
            result << res[i][j] << "," << i << endl;
        }
    }
    //Calculate Entropy
    calculateEntropy(res,classLabels,vectorModel,k);

    return;
}

int main(int argc, char* argv[]) {

    string stemClassFileName = argv[1];
    string criterionFunction = argv[2];
    string classLabelsFileName = argv[3];
    int k = atoi(argv[4]);
    trials = atoi(argv[5]);
    outPutFileName = argv[6];
    //qualityFileName = argv[7];
    testFile = argv[8];

    if(criterionFunction == "SSE") {
        criterionFunctionFlag = 1;
    }else if(criterionFunction == "I2") {
        criterionFunctionFlag = 2;
    }else if(criterionFunction == "E1"){
        criterionFunctionFlag = 3;
    }

    ifstream classFile(classLabelsFileName);

    map<string,string> classLabels;
    vector<string> ids;
    string classToken;

    while(getline(classFile,classToken)) {
        istringstream ss(classToken);
        string temp,NEW_ID,label;
        int fl = 0;
        while(getline(ss,temp,',')) {
            if(fl == 0) {
                NEW_ID = temp;
                fl = 1;
            }else {
                label = temp;
            }
        }
        ids.push_back(NEW_ID);
        classLabels.insert(pair<string,string>(NEW_ID,label));

    }
    classFile.close();

    ifstream stemClass(stemClassFileName);
    map<string,vector<double> > vectorModel;
    map<string,double> vectorModelSqSum;
    string tag;
    vector<double> values;
    int isHeader = 0;
    while(getline(stemClass,tag)){

        if(isHeader == 1) {
            istringstream ss(tag);
            string temp,id;
            double val;
            int fl = 0;
            size_t sz;
            while(getline(ss,temp,',')) {
                if(fl == 0) {
                    id = temp;
                }else if(fl == 2) {
                    val = stod(temp,&sz);
                }
                fl++;
            }
            vectorModel[id].push_back(val);
            double sqSum = vectorModelSqSum[id];
            sqSum += val*val;
            vectorModelSqSum[id] = sqSum;
        }else {
           isHeader = 1;
        }
    }
    // out.open(testFile);
    labelFile.open("clusterQuality.csv");
    //out << vectorModel.size() << endl;
    //out << vectorModel["1"].size() << endl;
    // out << vectorModelSqSum["1"] << endl;

    map<string,vector<double> >:: iterator sit;
    map<string,double>:: iterator tm;

    clock_t st,en;
    st = clock();
    for(sit = vectorModel.begin();sit != vectorModel.end();sit++) {
        double dv = vectorModelSqSum[(*sit).first];
        vector<double> vec = (*sit).second;
        int len = vec.size();
        for(int i = 0;i < len;i++) {
            vec[i] /= sqrt(dv);
        }
        (*sit).second = vec;
    }
    en = clock();
    //out << " Time Taken " << ((double)(en-st))/CLOCKS_PER_SEC << endl;

    clock_t kmeanS,kmeanE;
    kmeanS = clock();
    kMeans(ids,&classLabels,&vectorModel, k, trials);
    kmeanE = clock();
    labelFile << "Total RunTime " << ((double)(kmeanE-kmeanS))/CLOCKS_PER_SEC << "\n";
	return 0;
}
