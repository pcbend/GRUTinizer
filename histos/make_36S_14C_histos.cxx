#include "TRuntimeObjects.h"
#include "TChannel.h"

#include "TFSU.h"
#include "GCutG.h"

//boundries for square gates

const double alpha_background_timing_low   = 10.0;
const double alpha_background_timing_high  = 50.0;
const double alpha_prompt_timing_low       = -40.0;
const double alpha_prompt_timing_high      = 0.0; 

const double proton_background_timing_low  = 20.0;
const double proton_background_timing_high = 50.0;
const double proton_prompt_timing_low      = -25.0;
const double proton_prompt_timing_high     = 5.0;

//Cutoff energy of the dE for an alpha
const double dE_alpha_cutoff               = 600.0;

//Subscript is assigned once in the set_subscript function
char subscript;

enum particle_type_enum{PROTON, ALPHA, NO_PARTICLE};

class GateInfo
{
  int  particle_type;
  GCutG *gg_timing_background;
  GCutG *gg_timing_prompt;
  GCutG *gamma_dE_timing;
public:
  void initialize_values(TRuntimeObjects &obj);
  int set_particle_type(TRuntimeObjects &obj, double dE_energy, double E_energy);
  int get_particle_type();
  GCutG* get_gg_timing_background();
  GCutG* get_gg_timing_prompt();
  GCutG* get_gamma_dE_timing(); 
};

void GateInfo::initialize_values(TRuntimeObjects &obj)
{
  TFSU  *fsu_event = obj.GetDetector<TFSU>();
  TFSUHit E  = fsu_event->GetE(); 
  TFSUHit dE = fsu_event->GetDeltaE();

  TList *gates = &(obj.GetGates());
  int num_gates = gates->GetEntries();

  particle_type = set_particle_type(obj, dE.GetEnergy(), E.GetEnergy());
  
  for(int i=0; i < num_gates; i++)
  {
    GCutG*       cut      = (GCutG*)gates->At(i);
    const char*  cut_name = cut->GetName();

    if(!strcmp(cut_name, "gg_timing_background"))
    {
      gg_timing_background = cut;
    }
    if(!strcmp(cut_name, "gg_timing_prompt"))
    {
      gg_timing_prompt = cut;
    }
    if(!strcmp(cut_name, "gamma_dE_timing_cut_better"))
    {
      gamma_dE_timing = cut;
    }
  }
}

//Get particle type will return the particle type (an enumerated integer). It will check in two ways, first if the E and dE are inside of the particle gates on the PID. If they are not, it will check if the dE had an energy greater than the alpha cutoff. If not, it will return as if no particle was detected.
int GateInfo::set_particle_type(TRuntimeObjects &obj, double dE_energy, double E_energy)
{

  //Check if there is no particle first, since this is relatively common. This will stop it from looping through unnecessarily
  if(dE_energy == -1)
  {
    return NO_PARTICLE;
  }

  TList *gates = &(obj.GetGates());
  int num_gates = gates->GetEntries();

  for(int i=0; i < num_gates; i++)
  {
    GCutG*       cut      = (GCutG*)gates->At(i);
    const char*  cut_name = cut->GetName();

    if(cut->IsInside(E_energy, dE_energy))
    {
      if(!strcmp(cut_name, "Alpha"))
      {
	return ALPHA;
      }
      if(!strcmp(cut_name, "Proton"))
      {
	return PROTON;
      }
    }

  }

  if(dE_energy > dE_alpha_cutoff)
  {
    return ALPHA;
  }
  else
  {
    return NO_PARTICLE;
  }

}

int GateInfo::get_particle_type()
{
  return particle_type;
}

GCutG* GateInfo::get_gg_timing_background()
{
  return gg_timing_background;
}

GCutG* GateInfo::get_gg_timing_prompt()
{
  return gg_timing_prompt;
}

GCutG* GateInfo::get_gamma_dE_timing()
{
  return gamma_dE_timing;
}

TFSUHit get_event_hit(TFSU* fsu_event, bool is_addback, unsigned int index)
{

  TFSUHit hit;

  if(is_addback == true)
  {
    hit = fsu_event->GetAddbackHit(index);
  }
  else
  {
    hit = fsu_event->GetFSUHit(index);
  }

  return hit;
}

unsigned int get_event_size(TFSU* fsu_event, bool is_addback)
{
  
  unsigned int size;

  if(is_addback == true)
  {
    size = fsu_event->AddbackSize();
  }
  else
  {
    size = fsu_event->Size();
  }

  return size;
}

//Generates all general (non-gated) histograms
void fill_general_histograms(TRuntimeObjects &obj)
{
  //Get the event
  TFSU  *fsu_event = obj.GetDetector<TFSU>();

  //Gets E and dE data
  TFSUHit E  = fsu_event->GetE(); 
  TFSUHit dE = fsu_event->GetDeltaE();

  //PID
  obj.FillHistogram("PID",2000,0,12000,E.GetEnergy(), 2000,0,12000,dE.GetEnergy());

  //1D PID, only plotting dE
  obj.FillHistogram("dE",2000,0,12000,dE.GetEnergy());

  //Energy for E, dE
  obj.FillHistogram("Energy_E", 2000,0,4000, E.GetEnergy());
  obj.FillHistogram("Energy_dE", 2000,0,4000, dE.GetEnergy());
}

// Returns a string of the proper histogram directory name, depending on the number of dimensions of the histogram and the particle type.
char* get_directory_name(int dimensions, GateInfo* gates)
{

  int particle_type = gates->get_particle_type();

  if(particle_type == PROTON)
  {
    return Form("proton_%dD_histograms", dimensions);
  }
  else if(particle_type == ALPHA)
  {
    return Form("alpha_%dD_histograms", dimensions);
  }
  else
  {
    return Form("no_particle_%dD_histograms", dimensions);
  }
}

//Gamma gamma timing is plotted by taking the time difference of the higher energy vs the lower energy as the x axis, and the lower energy of the two hits as the y axis. 
void make_gamma_gamma_timing(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, double dT, double dT2, GateInfo* gates, char* directory)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;

  int gamma_dT;
  double lower_energy;

  if(hit.GetEnergy() < hit2.GetEnergy())
  {
    lower_energy = hit.GetEnergy();
    gamma_dT = hit2.GetTime() - hit.GetTime();
  }
  else
  {
    lower_energy = hit2.GetEnergy();
    gamma_dT = hit.GetTime() - hit2.GetTime();
  }

  obj.FillHistogram(directory, Form("%c%c_timing_mat", subscript, subscript), 2000, -1000, 1000, gamma_dT,
                                                                              6000, 0, 6000, lower_energy);
}

void make_1D_prompt_random(TRuntimeObjects &obj, TFSUHit* phit, double dT, GateInfo* gates, char* directory)
{

  TFSUHit hit = *phit;

  int particle_type = gates->get_particle_type();

  if(particle_type == PROTON)
  {
    if(dT<=proton_prompt_timing_high && dT>=proton_prompt_timing_low)
    {
      obj.FillHistogram(directory, Form("%c_singles_prompt_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
      obj.FillHistogram(directory, Form("%c_bsub_singles_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
    }
    else if(dT<=proton_background_timing_high && dT>=proton_background_timing_low)
    {
      obj.FillHistogram(directory, Form("%c_singles_random_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
      obj.FillHistogram(directory, Form("%c_bsub_singles_rectangle_gate", subscript), 12000,0,12000, (-1.0 * hit.GetEnergy()));
    }
  }
  
  if(particle_type == ALPHA)
  {
    if(dT<=alpha_prompt_timing_high && dT>=alpha_prompt_timing_low)
    {
      obj.FillHistogram(directory, Form("%c_singles_prompt_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
      obj.FillHistogram(directory, Form("%c_bsub_singles_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
    }
    else if(dT<=alpha_background_timing_high && dT>=alpha_background_timing_low)
    {
      obj.FillHistogram(directory, Form("%c_singles_random_rectangle_gate", subscript), 12000,0,12000, hit.GetEnergy());
      obj.FillHistogram(directory, Form("%c_bsub_singles_rectangle_gate", subscript), 12000,0,12000, (-1.0 * hit.GetEnergy()));
    }
  }
}

void fill_1D_histograms(TRuntimeObjects &obj, TFSUHit* phit, double dT, TFSUHit* pdE, GateInfo* gates)
{

  TFSUHit hit = *phit;
  TFSUHit dE  = *pdE;

  char* directory = get_directory_name(1, gates);

  //Make a 1D histogram of all gammas
  obj.FillHistogram(directory, Form("%c_singles", subscript), 12000,0,12000, hit.GetEnergy());
  obj.FillHistogram(directory, Form("%c_summary", subscript), 12000,0,12000, hit.GetEnergy(), 50,0,50, hit.GetNumber());

  make_1D_prompt_random(obj, phit, dT, gates, directory);

  if(hit.GetEnergy() > 20 && hit.GetEnergy() < 9000)
  {

    //Jk I know these are 2D but they only use 1 gamma
    obj.FillHistogram(directory, "particle_gamma_timing_spectrum", 200,-100,100,(dE.GetTime() - hit.GetTime()), 2000,0,4000, hit.GetEnergy());
    obj.FillHistogram(directory, "particle_gamma_detector_spectrum", 200,-100,100,(dE.GetTime() - hit.GetTime()), 50,0,50, hit.GetNumber());
    obj.FillHistogram(directory, "gamma_detector_num_spectrum", 10000, 0, 10000, hit.GetEnergy(), 50, 0, 50, hit.GetNumber());
  }
}


void fill_2D_histograms(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, double dT, double dT2, GateInfo* gates)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;
  
  char* directory = get_directory_name(2, gates);

  make_gamma_gamma_timing(obj, phit, phit2, dT, dT2, gates, directory);

  obj.FillHistogramSym(directory, Form("%c%c_mat", subscript, subscript), 4000, 0, 4000, hit.GetEnergy(), 4000, 0, 4000, hit2.GetEnergy());
}

// General function for iterating over energies of interest when populating the 3D histograms
void iterate_over_energies_and_fill_3D(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3, double dT, double dT2, GateInfo* gates, int* energies_to_gate, int energies_to_gate_size, int uncertainty)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;
  TFSUHit hit3 = *phit3;

  char* directory = get_directory_name(3, gates);

  //Iterate over all proton energies of interest to gate on
  for(int i=0; i<energies_to_gate_size; i++)
  {
    int gate_energy = energies_to_gate[i];

    if(fabs(hit3.GetEnergy()-gate_energy) <= uncertainty)
    {
      obj.FillHistogramSym(directory, Form("%c%c%c_cube_%d", subscript, subscript, subscript, gate_energy), 4000, 0, 4000, hit.GetEnergy(), 
			   4000, 0, 4000, hit2.GetEnergy()); 
      obj.FillHistogramSym(directory, Form("%c%c%c_4096_cube_%d", subscript, subscript, subscript, gate_energy), 4096, 0, 4096, hit.GetEnergy(), 
			   4096, 0, 4096, hit2.GetEnergy()); 
    }
  }
}

void fill_3D_histograms(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3, double dT, double dT2, GateInfo* gates)
{

  /*
    General notes about the methods of generating 3D histograms:
    A true 3D histogram is not generated. Instead, pre-selected energies are gated on here in the sorting process. Those energies are shown in 
    the array energies_to_gate. A 2D matrix will be generated on all gamma rays coincident with the energies in energies_to_gate, effectively 
    taking a slice out of a 3D cube.
  */

  int particle_type = gates->get_particle_type();

  //energies_to_gate is an array that contains the centroids of interest of a cube coincidence. 
  int proton_energies_to_gate_size = 4;
  int proton_energies_to_gate[] = {370, 520, 1001, 1147};

  int alpha_energies_to_gate_size = 3;
  int alpha_energies_to_gate[] = {1554, 1324, 1064};
  
  //Uncertainty determined the energy range in which a gamma ray will be considered (e.g. an uncertainty of 2 will consider allow a 1552 to be considered as a 1554)
  int uncertainty = 2;


  if(particle_type == PROTON)
  {
    iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, proton_energies_to_gate, proton_energies_to_gate_size, uncertainty);
  }

  if(particle_type == ALPHA)
  {
    iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, alpha_energies_to_gate, alpha_energies_to_gate_size, uncertainty);
  }
}

//Set the global variable "subscript" to either a or g. This is used in the titling of histograms.
void set_subscript(bool is_addback)
{
  //"a" for addback, "g" for gamma
  if(is_addback == true)
  {
    subscript = 'a';
  }
  else
  {
    subscript = 'g';
  }
}

//Make all histograms, with the condition on if they are addback or not. 
void make_histograms(TRuntimeObjects &obj, bool is_addback)
{

  set_subscript(is_addback);

  fill_general_histograms(obj);
 
  //Get the event
  TFSU  *fsu_event = obj.GetDetector<TFSU>();

  //Make the addback hits from the event. An addback his is similar to a regular hit, but certain processing has been 
  //performed to control for things like compton effects and scattering.
  fsu_event->ClearAddback();
  fsu_event->MakeAddbackHits();

  //Gets E and dE data
  TFSUHit E  = fsu_event->GetE(); 
  TFSUHit dE = fsu_event->GetDeltaE();

  GateInfo gates;
  gates.initialize_values(obj);

  unsigned int size = get_event_size(fsu_event, is_addback);
  
  for(unsigned int i=0; i < size; i++) 
  {

    //Get the hit, find the timing difference between dE and the gamma, the populate
    TFSUHit hit  = get_event_hit(fsu_event, is_addback, i);
    double dT = dE.GetTime() - hit.GetTime();
    fill_1D_histograms(obj, &hit, dT, &dE, &gates);
    
    /*
    //Check if there are atleast 2 hits for 2D analysis
    if(size >= 2)
    {
      for(unsigned int j=i+1; j < size; j++)
      {

	//Get the hit, find the timing difference between dE and the gamma, the populate
	TFSUHit hit2 = get_event_hit(fsu_event, is_addback, j);
	double dT2 = dE.GetTime() - hit2.GetTime();
	fill_2D_histograms(obj, &hit, &hit2, dT, dT2, &gates);
	
	if(size >= 3)
	{
	  for(unsigned int k=j+1; k<size; k++)
	  {

	    TFSUHit hit3 = get_event_hit(fsu_event, is_addback, k);
	    fill_3D_histograms(obj, &hit, &hit2, &hit3, dT, dT2, &gates);
	   
	  }
	}
      }
    }
    */
  }
}


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) 
{

  /* 
     The obj passed into MakeHistograms contains one events, along with the gates.

     An event is a grouping of hits based on a time window defined at build time. 
     An event contains two pieces of information at the top level:

     Member            Code                         Comment
     E                 fsu_event->GetE()
     dE                fsu_event->GetDeltaE()
     size              fsu_event->Size()            Returns the total number of hits in fsu_event
     hit               fsu_event->GetFSUHit(index)  Returns a specific hit at an index

     E and dE contain multiple members themselves:

     Member            Code            Comment
     Timestamp         E.Timestamp()   Timestamp is in units of 10's of nanoseconds
     Energy            E.GetEnergy()   Energy is the calibrated charge
     Charge            E.GetCharge()   Charge is charge of the detector

     An event contains multiple hits which can be indexed using the GetFSUHit(index) member of TFSU
     Each hit contains multiple parts, which can be indexed using:

     Member            Code            Comment
     Charge            hit.Charge()  
     Timestamp         hit.Timestamp() Timestamp is in units of 10's of nanoseconds
     Detector Name     hit.GetName()
     Energy            hit.GetEnergy()
     Detector Number   hit.GetNumber()
     Detector Address  hit.Address()
     */

  /*
     A gate is a cutout of any 2D plot (Generally the PID). A series of gate objects are stored
     as a TList. An individual gate (or a cut) is stored as a GCutG. 
     have multiple members:

     Member             Code                Comment
     Number of Gates    gates->GetEntries()
     Specific Gate      gates->At(index)    returns a specific gate (aka a "cut")
     Name               cut->GetName()      returns the name of the cut (i.e. "Proton", "Alpha")
     */


  //Get event and gates. 
  TFSU  *fsu_event = obj.GetDetector<TFSU>();

  //Check if event exists
  if(!fsu_event)
  {
    return;
  }

  make_histograms(obj, false);
  make_histograms(obj, true);

}
