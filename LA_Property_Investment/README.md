Abstract:
We utilize machine learning algorithms through supervised learning to determine which types of properties are of interest to an investor
and provide property recommendations.

Instructions:
1. The CSV file contains property information and should be included in the property directory before running the CPP file.
2. The program either asks for the user to enter the name of the file they wish to open or will automatically be set to open
the np2.csv file.  If you choose to use another CSV be sure to change the fin.open("X") where "X" is the name of the file you
wish to open.
3. The program takes steps to estimate and update propery values which should take roughly O(n^2) or less time per step.
4. Then the program should ask you to provide filtering values for min and max SQFT, bedrooms, bathrooms, and price.
5. Next, there should be supervised testing where the program asks you to determine which qualities of the home you like or dislike
the most.  This step provides you random homes.  For each decision it will adjusts the weights for each factor. 
6. Finally, the program will recommend a property.  If you like this property, the program will terminate.  If you dislike the property,
the program will continue until you like property.

Future considerations:
-I would like to incorporate a feature that will weigh the later decisions more heavily than the first.  In other words, I think the
choices you make after viewing more properties is more valuable, educated than the decisions made earlier.
-Another great problem is comparing properties that aren't within a certain proximity to one another.  I had to limit my data size
to properties in one zip code, but I must think more deeply about how to compare properties across zip codes and other regions.
-With respect to supervised learning, there is a tradeoff between the speed and predictive powers of the program: the faster the program,
the less predictive and accurate the recommendation.  I chose to make the testing only consist of a few properties to simply demonstrate
the underlying logic and intent of the program.  In the future I might expand the testing period.
