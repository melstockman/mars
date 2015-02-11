/* For the mission to Mars some areas are chosen, where soil, climate, and seismology investigations will be made. 
   These areas are of size 10 000 × 10 000 meters. The probes are sent to all these areas. For the effective work, 
   the probes in each area must be connected to high-speed data network.

   Since delivery to Mars is very expensive, you have to determine the minimum length of the optical fiber cable which 
   connects probes to a network.

INPUT SAMPLE:

The first argument is a file that contains the space-separated coordinates of the site’s probes, 
one line per site. X and Y coordinates are separated by comma.

For example:

500,8000 1000,9500 2000,8500 1000,7500 4500,7000 5500,6500 7000,7000 2500,4000 1000,4000 1000,3000 3000,2500 2500,1000 3500,500 9000,6000 8500,4500 7500,4000 9000,3500 10000,3000
8028,5930 1835,5145 8537,9824 7623,7936 8031,1207
9349,3367 395,3342 3588,3655 9235,2503 1075,6413 2394,8353
9013,3937 7791,872 2417,3183
3416,472 8093,7510 1709,4893 9999,6958 6761,2692 2530,6753

OUTPUT SAMPLE:

Print to stdout the minimum length of optical fiber cable for every site:

26602
15110
15335
9150
17770
If the length is not integer (for example, 9063. 114), you should round it upward (9064).

CONSTRAINTS:

Number of sites is 20.
Number of probes within one site can be from 3 to 250.
There can be 2 or more probes with the same coordinates — in such case they do not need any cable to interconnect.

*/

#include <iostream>
#include <math.h>

using namespace std;

#define NUMSITES 5
#define MAXPROBES 25

class PROBE {
	public:
		int x;
		int y;
		int connected;
};
class SITE {
    public:
	   PROBE probe[MAXPROBES];
	   double distance[MAXPROBES][MAXPROBES]; /* distance[i][j]is the distance between node i and node j */
	   double distanceFromSpanningTree[MAXPROBES];  /* distance between probe[i] and minimum spanning tree */
	   int whoTo[MAXPROBES];
	   double totalDistance;
	   int numProbes;
};
double distanceBetween( PROBE p1, PROBE p2 )
{
	return sqrt( (double)((p1.x-p2.x)*(p1.x-p2.x)) + (double)((p1.y-p2.y)*(p1.y-p2.y)) );
}

/* updateDistances(int target)
    should be called immediately after target is added to the tree;
    updates d so that the values are correct (goes through target's
    neighbours making sure that the distances between them and the tree
    are indeed minimum)
*/
void updateDistances(int target, SITE *site, int n) {
    int i;
    for (i = 0; i < n; ++i)
        if ((site->distance[target][i] != 0) && (site->distanceFromSpanningTree[i] > site->distance[target][i]))
		{
            site->distanceFromSpanningTree[i] = site->distance[target][i];
            site->whoTo[i] = target;
        }
}

SITE site[NUMSITES];

void readInSites( char *inFile )
{
    FILE *fp;
	int i,probeIndex;
	char c;

	fp = fopen(inFile,"r");
	if (!fp)
	{
		printf("Can't open file %s\n", inFile);
		exit(1);
	} // if
	for (i=0;i<NUMSITES;i++)
	{
	  probeIndex = 0;
      
      while (fscanf(fp, "%d,%d", &site[i].probe[probeIndex].x, &site[i].probe[probeIndex].y) == 2) // expect 2 successful conversion
      {   
          //printf("%d,%d ", site[i].probe[probeIndex].x, site[i].probe[probeIndex].y);
		  
		  site[i].probe[probeIndex].connected = 0;
		  site[i].whoTo[probeIndex] = -1;

		  probeIndex++;

          c = fgetc(fp);
		  if (c=='\n' || c==10)
			  break;
      } // while
	  //printf("\n");
	
	  site[i].numProbes = probeIndex;
	
   } // i
	 
   fclose(fp);
}
void makeProbeTree( int i )
{
	 int j,k,q,probeIndex,treeIndex;

     // look throught all the probes and find the closest for each one
	 // initialize

	  probeIndex = site[i].numProbes;

      for (j=0; j<probeIndex; j++)
	  {
		  site[i].distanceFromSpanningTree[j] = 99999999;
		  for (k=0;k<probeIndex;k++)
			  site[i].distance[j][k] = distanceBetween(site[i].probe[j],site[i].probe[k]);
	  }
      site[i].totalDistance = 0;
      for (treeIndex = 1; treeIndex <= probeIndex; ++treeIndex) 
	  {
        /* Find the node with the smallest distance to the tree */
        int min = -1;
        for (q = 0; q < probeIndex; ++q)
		{
            if (!site[i].probe[q].connected)
			{
                if ((min == -1) || (site[i].distanceFromSpanningTree[min] > site[i].distanceFromSpanningTree[q]))
				{
                    min = q;
				}
			}
		}
 		updateDistances(min, &site[i], probeIndex);
 		site[i].probe[min].connected  = 1;
		/* And add it */
        //printf("Adding edge (%d,%d)-(%d,%d)\n", site[i].probe[site[i].whoTo[min]].x,site.probe[site[i].whoTo[min]].y,
		//	                                    site[i].probe[min].x, site[i].probe[min].y);
        if (min) // get past first node
		   site[i].totalDistance += site[i].distanceFromSpanningTree[min];
 
	  } // for treeSize
}
void removeFaultyProbe( int faultySiteNum, int faultyProbeNum)
{
	int i, probeIndex;

	probeIndex = site[faultySiteNum].numProbes;
	site[faultySiteNum].totalDistance = 0;

	// remove connections
	for (i=0;i<probeIndex;i++)
	{
		site[faultySiteNum].probe[i].connected = 0;
		site[faultySiteNum].whoTo[i] = -1;
	}

    for (i=faultyProbeNum;i<probeIndex-1;i++)
	{
		site[faultySiteNum].probe[i].connected = site[faultySiteNum].probe[i+1].connected;
		site[faultySiteNum].probe[i].x = site[faultySiteNum].probe[i+1].x;
		site[faultySiteNum].probe[i].y = site[faultySiteNum].probe[i+1].y;
	}


	site[faultySiteNum].numProbes--;

}
int main(int argc, char **argv)
{
	  int i,faultySiteNum,faultyProbeNum;
	
	  readInSites(argv[1]);
	  
	  for (i=0;i<NUMSITES;i++)
	  {
		  makeProbeTree(i);
	      printf("%.0f\n", ceil(site[i].totalDistance) );
      } // for i

	  printf("Enter faulty probe 'faultySiteNum faultyProbeNum': ");

	  scanf("%d %d", &faultySiteNum, &faultyProbeNum);

	  faultySiteNum--;
	  faultyProbeNum--;

	  removeFaultyProbe(faultySiteNum,faultyProbeNum);

	  makeProbeTree(faultySiteNum);
	  printf("%.0f\n", ceil(site[faultySiteNum].totalDistance) );
	
      return 0;
} // main
 
