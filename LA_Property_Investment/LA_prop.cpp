#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include "math.h"
using namespace std;

//PROPERTY OBJECT
class Property{
public:
    long AIN;
    int Prop_code;
    string general_use;
    string specific_use;
    int year_built = 2017;
    int effective_year_built;
    int SQFT_home = 0;
    int num_bed;
    int num_bath;
    int recording_date;
    double land_value;
    int land_base_year;
    double imp_value;
    int imp_base_year;
    int tot_value;
    int address_number;
    string address_street;
    string address_city;
    int address_zip;
    double center_lat;
    double center_long;
};
/////////////////////////////////////////////////////////////////////////////////////
//CREATING PROPERTY OBJECTS AND DATABASE
bool add_feature (int col_num, Property & new_prop, string prop_field);
void create_property_DB(fstream & fin, string & line, vector <Property> & prop_list, vector <Property> & invest_list);

//PROPERTY COMPARABILITY FUNCTIONS
double deg2rad(double deg){ return deg * (M_PI/180);}
//Haversine formula: algorithm from stackflow to calculate distance between two properties
double get_distance(double lat1, double lon1, double lat2, double lon2);
void prop_comparability(vector <Property> & prop_list);

//FILTER LIST according to user preference
void filter_list (vector <Property> & filt_prop_list, vector <Property> prop_list);

//SUPERVISED LEARNING
void get_ratio_mult_resi_prop(vector <Property> invest_list, vector <Property> filt_list, Property cand, int & resi_count, int & mult_count, double mult_dis);
void adj_weights(vector <Property> & filt_prop_list, int best_index, float & user_mult_unit_w, float & user_flip_w, float & user_price_w, float & user_year_w);
void test_rand_prop (vector <Property> & filt_prop_list, int best_index, float & user_mult_unit_w, float & user_flip_w, float & user_price_w, float & user_year_w);

//RECOMMENDATION
int find_cand(vector <Property> filt_prop_list, vector <Property> invest_list, Property & best, float & user_price_w, float & user_flip_w, float & user_mult_unit_w, float & user_year_w);

/////////////////////////////////////////////////////////////////////////////////////
int main() {
    
    //OPEN DATA FILE
    string line;
    fstream fin;
    fin.open("np2.csv");
    getline(fin, line);
    
    //VARIABLES
    Property best;
    int best_index = 0;
    
    //initial weights
    float user_price_w = 1;
    float user_flip_w = .75;
    float user_mult_unit_w = 1;
    float user_year_w = 1;
    
    //while condition
    bool prop_not_found = 1;
    
    //VECTORS
    vector <Property> invest_list;
    vector <Property> prop_list;
    vector <Property> filt_prop_list;
    
///////////////////////////////////////////////////////////////////////////////////////////////

    //create database
    create_property_DB(fin, line, prop_list, invest_list);

///////////////////////////////////////////////////////////////////////////////////////////////
    
    //apples to apples: estimate property values using recently sold neighbor information
    prop_comparability(prop_list);
    
///////////////////////////////////////////////////////////////////////////////////////////////
    
    //filter list according to user preferences
    filter_list(filt_prop_list, prop_list);
    
///////////////////////////////////////////////////////////////////////////////////////////////
    
    while (prop_not_found){
        //TESTING: WEIGHT ADJUSTMENT
        test_rand_prop (filt_prop_list, best_index, user_mult_unit_w, user_flip_w, user_price_w, user_year_w);
        
        //FIND BEST PROPERTY
        best_index = find_cand(filt_prop_list, invest_list, best, user_price_w, user_flip_w, user_mult_unit_w, user_year_w);
        
        cout << "Here is the best:" << endl;
        cout << best.address_number << " " << best.address_street << " " << best.tot_value << endl;

        cout << "printing final weights: " <<endl
             << "multiple units weight: " << user_mult_unit_w << endl
             << "user flip weight:  " << user_flip_w << endl
             << "user price weight: " << user_price_w << endl
             << "user year weight: " << user_year_w << endl;
        
        cout << "do you want to continue: 1 for yes, 0 for no" << endl;
        cin >> prop_not_found;
    }
    
    return 0;
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool add_feature (int col_num, Property & new_prop, string prop_field){
    
    switch (col_num){
        case 0: new_prop.AIN = stol(prop_field);
            break;
        case 1: new_prop.Prop_code = stoi(prop_field);
            break;
        case 2: new_prop.general_use = (prop_field);
            break;
        case 3: new_prop.specific_use = (prop_field);
            break;
        case 4: new_prop.year_built = stoi(prop_field);
            break;
        case 5: new_prop.effective_year_built = stoi(prop_field);
            break;
        case 6: new_prop.SQFT_home = stoi(prop_field);
            break;
        case 7: new_prop.num_bed = stoi(prop_field);
            break;
        case 8: new_prop.num_bath = stoi(prop_field);
            break;
        case 9: new_prop.recording_date = stoi(prop_field);
            break;
        case 10:
            new_prop.land_value = stod (prop_field);
            break;
        case 11:
            new_prop.land_base_year = stoi (prop_field);
            break;
        case 12:
            new_prop.imp_value = stod (prop_field);
            break;
        case 13:
            new_prop.imp_base_year = stoi(prop_field);
            break;
        case 14: new_prop.tot_value = stoi(prop_field);
            break;
        case 15: new_prop.address_number = stoi(prop_field);
            break;
        case 16: new_prop.address_street = (prop_field);
            break;
        case 17: new_prop.address_city = (prop_field);
            break;
        case 18: new_prop.address_zip = stoi(prop_field);
            break;
        case 19: new_prop.center_lat = stod(prop_field);
            break;
        case 20: new_prop.center_long = stod(prop_field);
            break;
        default: cout << "error" << endl;
            return false;
    }
    
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_property_DB(fstream & fin, string & line, vector <Property> & prop_list, vector <Property> & invest_list){
    
    int column_count = 0;
    string keyword = "";
    
    while (getline(fin, line)){
        Property new_prop;
        for (int i = 0; i < line.length(); i++){
            if (line[i] == '"'){
                i++;
                //all the doubles
                if (((column_count == 10) || (column_count == 12)) || (column_count == 14)){
                    i++;
                }
                while (line[i]!= '"'){
                    if (line[i]!=',')
                        keyword+=line[i];
                    i++;
                }
            }
            if ((line[i] == ',') || i+1 == line.length()){
                //cout << keyword << endl;
                if (!(add_feature (column_count, new_prop, keyword))) exit(0);
                keyword = "";
                column_count++;
            }
            else{
                if (!((line[i] == '"') || (line[i] == '$')))
                    keyword+=line[i];
            }
        }
        
        //if AIN of new_prop is the same as the AIN for the last property inserted in prop_list
        //then check if new_prop.land_value_year is the same as the last property's land_value_year
        //if its not
        
        
        if (new_prop.Prop_code > 100){
            invest_list.push_back(new_prop);
        }
        else{
            prop_list.push_back(new_prop);
        }
        
        column_count = 0;
    }//end while
    
    //return prop_list;
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double get_distance(double lat1, double lon1, double lat2, double lon2){
    
    int r = 6371;
    double dLat = deg2rad (lat2- lat1);
    double dLon = deg2rad (lon2 - lon1);
    
    double a = sin(dLat/2) * sin(dLat/2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = r * c; //in radian
    
    double toMiles = d * .62137;
    
    return toMiles;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void prop_comparability(vector <Property> & prop_list){
    cout << "Estimating Property Values" << endl;
    
    bool updated = false;
    int price_per_sq_foot = 0;
    Property cand;
    double sim_dist;
    double best_min_dist;
    
    for (int i = 0; i < prop_list.size(); i++){
        if (prop_list[i].land_base_year < 2016){
            int avoid_index = i;
            updated = false;
            sim_dist = 0;
            best_min_dist = 100000000;
            for (int k = 0; k < prop_list.size(); k++){
                if (((k!=avoid_index) && (prop_list[k].land_base_year >2016)) && (prop_list[k].Prop_code == prop_list[i].Prop_code)){
                    if (prop_list[k].num_bed > 0){
                        if (prop_list[k].SQFT_home > 0){
                            if (prop_list[k].tot_value > 100000){
                                sim_dist+=pow(1 + (get_distance(prop_list[i].center_lat, prop_list[i].center_long, prop_list[k].center_lat, prop_list[k].center_long)), 2);
                                sim_dist+=pow((prop_list[i].num_bed+prop_list[i].num_bath)/(prop_list[k].num_bed + prop_list[k].num_bath), 2);
                                sim_dist+=pow((prop_list[i].SQFT_home/prop_list[k].SQFT_home), 2);
                            }
                        }
                    }
                    
                    //if the candidate property is closer (in features) than previous candidate
                    if ((sim_dist < best_min_dist) && (sim_dist > 0)){
                        cand = prop_list[k];
                        best_min_dist = sim_dist;
                    }
                    sim_dist = 0;
                }//if not same property
            }//for k
            price_per_sq_foot = cand.imp_value/cand.SQFT_home;
            prop_list[i].imp_value = price_per_sq_foot * prop_list[i].SQFT_home;
            prop_list[i].land_value = cand.land_value;
            prop_list[i].tot_value = prop_list[i].imp_value + prop_list[i].land_value;
            //prop_list[i].land_base_year = 2017;
        }//if land base year is < 2016
    }//for i
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void filter_list (vector <Property> & filt_prop_list, vector <Property> prop_list){
    
    //VARIABLES FOR FILTERING
    int min_sqft = 0;
    int max_sqft = 100000;
    
    int min_num_bed = 0;
    int max_num_bed = 100;
    
    int min_num_bath = 0;
    int max_num_bath = 0;
    
    int min_price = 0;
    int max_price = 100000000;
    
    cout << "Please define the minimum square footage acceptable:  ";
    cin >> min_sqft;
    
    cout << "Please define the maximum square footage acceptable:  ";
    cin >> max_sqft;
    
    cout << "Please define the minimum number of bedrooms acceptable:  ";
    cin >> min_num_bed;
    
    cout << "Please define the maximum number of bedrooms acceptable:  ";
    cin >> max_num_bed;
    
    cout << "Please define the minimum number of bathrooms acceptable:  ";
    cin >> min_num_bath;
    
    cout << "Please define the maximum number of bathrooms acceptable:  ";
    cin >> max_num_bath;
    
    cout << "Please define the minimum total price acceptable:  ";
    cin >> min_price;
    
    cout << "Please define the maximum total price:  ";
    cin >> max_price;
    
    for (int i = 0; i < prop_list.size(); i++){
        if ((prop_list[i].SQFT_home >=min_sqft) && (prop_list[i].SQFT_home <=max_sqft)){
            if ((prop_list[i].num_bed >= min_num_bed) && (prop_list[i].num_bed <=max_num_bed)){
                if ((prop_list[i].num_bath >=min_num_bath) && (prop_list[i].num_bath <=max_num_bath)){
                    if ((prop_list[i].tot_value >=min_price) && (prop_list[i].tot_value <=max_price)){
                        if (prop_list[i].land_base_year > 0){
                            filt_prop_list.push_back(prop_list[i]);
                        }
                    }
                }
            }
        }
    }
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void get_ratio_mult_resi_prop(vector <Property> invest_list, vector <Property> filt_list, Property cand, int & resi_count, int & mult_count, double mult_dis){
    for (int i = 0; i < invest_list.size(); i++){
        if (get_distance(cand.center_lat, cand.center_long, invest_list[i].center_lat, invest_list[i].center_long) <=mult_dis){
            mult_count++;
        }
    }
    
    for (int k = 0; k < filt_list.size(); k++){
        if (get_distance(cand.center_lat, cand.center_long, filt_list[k].center_lat, filt_list[k].center_long) <=mult_dis){
            resi_count++;
        }
    }
    
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int find_cand(vector <Property> filt_prop_list, vector <Property> invest_list, Property & best, float & user_price_w, float & user_flip_w, float & user_mult_unit_w, float & user_year_w){
    
    Property cand;
    
    int best_index = 0;
    
    float price_w = 0;
    float flip_w = 0;
    float mult_w = 0;
    float year_w = 0;
    
    float cand_tot_w = 0;
    float best_tot_w = 0;
    
    double mult_dis = 1;
    int map_num_mult = 0;
    int map_num_resi = 0;
    
    for (int i = 0; i < filt_prop_list.size(); i++){
        cand = filt_prop_list[i];
        
        //price per square foot: assuming somewhere aroun 125 is reasonable but in the future should match the average home price per square foot
        price_w = user_price_w * 125/(filt_prop_list[i].tot_value/filt_prop_list[i].SQFT_home);
        
        //flip potential is maxing ratio of land value/imp value
        flip_w = user_flip_w * (filt_prop_list[i].land_value/filt_prop_list[i].imp_value);
        
        //maximizing the ratio of multi-unit homes near the property
        get_ratio_mult_resi_prop(invest_list, filt_prop_list, filt_prop_list[i], map_num_resi, map_num_mult, mult_dis);
        mult_w = user_mult_unit_w * (8*map_num_mult)/map_num_resi;
        
        //CREATE WEIGHT FOR ACTIVITY: COMPARE LAND BASE YEAR TO 1975
        year_w = user_year_w * (1975/(filt_prop_list[i].land_base_year));
        
        //total weight value
        cand_tot_w = price_w + flip_w + mult_w + year_w;
        
        //higher weight means better candidate property
        if (cand_tot_w > best_tot_w){
            best = cand;
            best_tot_w = cand_tot_w;
            cand_tot_w = 0;
            best_index = i;
            
        }
    }
    
    return best_index;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_rand_prop (vector <Property> & filt_prop_list, int best_index, float & user_mult_unit_w, float & user_flip_w, float & user_price_w, float & user_year_w){
    vector <Property> rand_props;
    
    for (int i = 0; i < 3; i++){
        srand(i);
        int rand_index = rand() % filt_prop_list.size();
        rand_props.push_back(filt_prop_list[rand_index]);
        cout << filt_prop_list[rand_index].address_number << " " << filt_prop_list[rand_index].address_street << " " << filt_prop_list[rand_index].tot_value << endl;
        //adjust weights here
        adj_weights(filt_prop_list, best_index, user_mult_unit_w, user_flip_w, user_price_w, user_year_w);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void adj_weights(vector <Property> & filt_prop_list, int best_index, float & user_mult_unit_w, float & user_flip_w, float & user_price_w, float & user_year_w){
    
    bool user_like = false;
    int user_sel = 0;
    
    cout << "did you like the property?: enter 1 for yes, and 0 for no" << endl;
    cin >> user_like;
    
    if (user_like){
        cout << "what did you like most about the property?" << endl
        << "Enter 1 for PROXIMITY TO SUBDIVIDED PROPERTIES" << endl
        << "Enter 2 for LAND" << endl
        << "Enter 3 for PRICE" << endl
        << "Enter 4 for YEAR/ACTIVITY" << endl;
        
    }
    else{
        //if not like remove property from list
        swap(filt_prop_list[best_index], filt_prop_list[filt_prop_list.size()-1]);
        filt_prop_list.pop_back();
        
        cout << "what did you dislike most about the property?" << endl
        << "Enter 1 for PROXIMITY TO SUBDIVIDED PROPERTIES" << endl
        << "Enter 2 for LAND" << endl
        << "Enter 3 for PRICE" << endl
        << "Enter 4 for YEAR/ACTIVITY" << endl;
    }
    
    cin >> user_sel;
    
    //scale for impact
    //set at 1.1 and initially raise to 1, then 2.... (exponent 1.1^1, then 1.1^2, 1.1^3)...here the 1, 2, 3 represent the number of times a property is judged
    
    if (user_like){
        if (user_sel == 1){
            user_mult_unit_w+=.10;
        }
        else if (user_sel == 2){
            user_flip_w+=.10;
        }
        else if (user_sel == 3){
            user_price_w+=.10;
        }
        else if (user_sel == 4){
            user_year_w +=.10;
        }
    }
    else{
        if (user_sel == 1){
            user_mult_unit_w+=.50;
        }
        else if (user_sel == 2){
            user_flip_w+=.50;
        }
        else if (user_sel == 3){
            user_price_w+=.50;
        }
        else if (user_sel == 4){
            user_year_w +=.50;
        }
    }
}


