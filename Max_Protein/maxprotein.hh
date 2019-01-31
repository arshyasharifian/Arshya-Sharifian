///////////////////////////////////////////////////////////////////////////////
// maxprotein.hh
//
// Compute the set of foods that maximizes protein, within a calorie budget,
// with the greedy method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// One food item in the USDA database.
//BEGIN CLASS
class Food {
private:
  // Human-readable description of the food, e.g. "all-purpose wheat
  // flour". Must be non-empty.
  string _description;

  // Human-readable description of the amount of the food in one
  // sample, e.g. "1 cup". Must be non-empty.
  string _amount;

  // Number of grams in one sample; must be non-negative.
  int _amount_g;

  // Energy, in units of kilocalories (commonly called "calories"), in
  // one sample; must be non-negative.
  int _kcal;

  // Number of grams of protein in one sample; most be non-negative.
  int _protein_g;

public:
  Food(const string& description,
       const string& amount,
       int amount_g,
       int kcal,
       int protein_g)
    : _description(description),
      _amount(amount),
      _amount_g(amount_g),
      _kcal(kcal),
      _protein_g(protein_g) {

    assert(!description.empty());
    assert(!amount.empty());
    assert(amount_g >= 0);
    assert(kcal >= 0);
    assert(protein_g >= 0);
  }

  const string& description() const { return _description; }
  const string& amount() const { return _amount; }
  int amount_g() const { return _amount_g; }
  int kcal() const { return _kcal; }
  int protein_g() const { return _protein_g; }
};

// Alias for a vector of shared pointers to Food objects.
typedef vector<shared_ptr<Food>> FoodVector;

// Load all the valid foods from a USDA database in their ABBREV
// format. Foods that are missing fields such as the amount string are
// skipped. Returns nullptr on I/O error.
unique_ptr<FoodVector> load_usda_abbrev(const string& path) {

	unique_ptr<FoodVector> failure(nullptr);
  
	ifstream f(path);
	if (!f) {
	return failure;
	}
  
	unique_ptr<FoodVector> result(new FoodVector);

	for (string line; getline(f, line); ) {

			vector< string> fields;
			stringstream ss(line);
		for ( string field;  getline(ss, field, '^'); ) {
			fields.push_back(field);
		}
		/*
			
		if (fields.size() != 53) {
			return failure;
		}
		*/
		
		string descr_field = fields[1],
				kcal_field = fields[3],
				protein_g_field = fields[4],
				amount_g_field = fields[48],
				amount_field = fields[49];

		auto remove_tildes = []( string& output,
					const string& field) {
			if ((field.size() < 3) ||
			(field.front() != '~') ||
			(field.back() != '~')) {
		return false;
			} else {
		output.assign(field.begin() + 1, field.end() - 1);
		return true;
			}
		};

		auto parse_mil = [](int& output, const  string& field) {
			stringstream ss(field);
			double floating;
			ss >> floating;
			if ( ! ss ) {
		return false;
			} else {
		output = lround(floating);
		return true;
			}
		};

			string description, amount;
		int amount_g, kcal, protein_g;
		if ( remove_tildes(description, descr_field) &&
			remove_tildes(amount, amount_field) &&
			parse_mil(amount_g, amount_g_field) &&
			parse_mil(kcal, kcal_field) &&
			parse_mil(protein_g, protein_g_field) ) {
			result->push_back( shared_ptr<Food>(new Food(description,
									amount,
									amount_g,
									kcal,
									protein_g)));
		}
	}
	f.close();
	return result;
}

// Convenience function to compute the total kilocalories and protein
// in a FoodVector. Those values are returned through the
// first two pass-by-reference arguments.
void sum_food_vector(int& total_kcal,
		     int& total_protein_g,
		     const FoodVector& foods) {
  total_kcal = total_protein_g = 0;
  for (auto& food : foods) {
    total_kcal += food->kcal();
    total_protein_g += food->protein_g();
  }
}

// Convenience function to print out each food in a FoodVector,
// followed by the total kilocalories and protein in it.
void print_food_vector(const FoodVector& foods) {
  for (auto& food : foods) {
     cout << food->description()
	      << " (100 g where each " << food->amount()
	      << " is " << food->amount_g() << " g)"
	      << " kcal=" << food->kcal()
	      << " protein=" << food->protein_g() << " g"
	      <<  endl;
  }
  
  int total_kcal, total_protein_g;
  sum_food_vector(total_kcal, total_protein_g, foods);
   cout << "total kcal=" << total_kcal
	    << " total_protein=" << total_protein_g << " g"
	    <<  endl;
}

//Filters food items using criteria passed in through function
 unique_ptr<FoodVector> filter_food_vector(const FoodVector& source,
					       int min_kcal,
					       int max_kcal,
					       int total_size) {
	unique_ptr<FoodVector> filtered_Food(new FoodVector);
	for (int i = 0; i < source.size(); i++) {
		if ((source[i]->kcal() >= min_kcal) && (source[i]->kcal() <= max_kcal))
			if (filtered_Food->size() < total_size)
				filtered_Food->push_back(source[i]);
	}
	return filtered_Food;
}

//in-place_selection_sort used in greedy_max_protein function
 void in_place_selection_sort(FoodVector & v) {
	for (int k = 0; k < (v.size() - 1); k++) {
		int least_i = k;
		for (int j = k + 1; j <= (v.size() - 1); j++) {
			if (v[j]->protein_g() < v[least_i]->protein_g())
				least_i = j;	 
		}
		if (k != least_i)
		swap(v[least_i], v[k]);
	}
 }
 /*
 This function uses the greedy method to find a vector of
 foods items that maximizes protein
 */
 unique_ptr<FoodVector> greedy_max_protein(const FoodVector& foods,
	 int total_kcal) {
	 //making copy of foods
	 FoodVector foods_copy;
	 for (auto& food : foods) {
		 foods_copy.push_back(food);
	 }
	 //sorting foods_copy
	 in_place_selection_sort(foods_copy);

	 int result_cal = 0;
	 unique_ptr<FoodVector> greedy_Foods(new FoodVector);
	 /*
	 since we sorted foods_copy from least to greatest with regard to protein,
	 we will insert foods items into our greedy_Foods Foodvector starting from
	 the food with the greatest number of protein, the last food item.
	 */
	 for (int i = foods_copy.size() - 1; i >= 0 ; i--) {
		 if (foods_copy[i]->kcal() + result_cal <= total_kcal) {
			 greedy_Foods->push_back(foods_copy[i]);
			 result_cal += foods_copy[i]->kcal();
		 }
	 }
	 return greedy_Foods;
}
 
 /*
 given the max number of food items, n, this function finds the 
 food item(s) that maximize protein using an exhaustive search
 for the best candidate.
 */
 unique_ptr<FoodVector> exhaustive_max_protein(const FoodVector& foods,
						   int total_kcal) {
  const int n = foods.size();
  assert(n < 64);

  //local variables
  int candidate_cal = 0, candidate_pro = 0;
  int best_pro = 0;
  //exh_Foods to ultimately hold best candidate
  unique_ptr<FoodVector> exh_Foods(new FoodVector);
  FoodVector best;

  //implementation of subsets
  for (uint64_t bits = 0; bits <= (uint64_t)((pow(2, n))-1); bits++) {
	  FoodVector candidate;
	  //a subset
	  for (int j = 0; j < n; j++) {
		  if (((bits >> j) & 1) == 1)
			candidate.push_back(foods[j]);
	  }
	  //tabulate total calories and protein for candidate
	  sum_food_vector(candidate_cal, candidate_pro, candidate);
	  if (candidate_cal <= total_kcal) {
		  if (best.size() == 0 || candidate_pro > best_pro) {
			  best.clear();
			  for (int k = 0; k < candidate.size(); k++)
				  best.push_back(candidate[k]);
			  best_pro = candidate_pro;
		  }
	  }
  }
  //store final best candidate in exh_Foods vector
  for (int i = 0; i < best.size(); i++)
	  exh_Foods->push_back(best[i]);
  
  return exh_Foods;
}
