#include "TRuntimeObjects.h"
#include "TChannel.h"

#include "TFSU.h"
#include "GCutG.h"

/********************************************************************

This file is for the sorting of the 36S(14C, np\alpha\gamma)50Ti reaction 
performed at FSU. This file will generate 1D, 2D, and 3D histograms when
GRUT.HistLib (located in .grutrc in the base directory) is set to this file.

example: GRUT.HistLib: $(GRUTSYS)/lib/libmake_36S_14C_histos.so

 ********************************************************************/

//Subscript is assigned once in the set_subscript function
char subscript;

//partilce_type_enum is used to easily reference the partilce type. For every event, only 1 particle is considered, so this is determined once and carried through every function.
enum particle_type_enum{PROTON, ALPHA, NO_PARTICLE};

/*Bounds of energy_low_cutoff and energy_high_cutoff determined by observation. Below 20keV, a large spike in gamma noise was observed, and above 9000keV, detector artifacts were observed.
  Because of this, a low and high bounds were put on the spectrum to clean things up.*/
const int energy_low_cutoff  = 20.;
const int energy_high_cutoff = 9000.;

/*GateInfo stores the gates located in the .cuts file fed in as a command line argument in a more easily accessible manner. When GateInfo::initialize_values is called, it will loop through
all gates and do a string compare to find gates of interest, and then store them in the private GCutG* variables. GateInfo::initialize_values will also determine and assign the particle type.
All private variables can be accessed by the public get_x functions of GateInfo. */
class GateInfo
{
  int  particle_type;

  GCutG *gg_timing_background;
  GCutG *gg_timing_prompt;
  GCutG *aa_timing_prompt;
  GCutG *aa_timing_background;

  GCutG *gp_timing_prompt;
  GCutG *gp_timing_background;

  GCutG *gamma_dE_timing;

public:
  void initialize_values(TRuntimeObjects &obj);
  int set_particle_type(TRuntimeObjects &obj, double dE_energy, double E_energy);

  int get_particle_type();

  GCutG* get_gg_timing_background();
  GCutG* get_gg_timing_prompt();
  GCutG* get_aa_timing_background();
  GCutG* get_aa_timing_prompt();

  GCutG* get_gp_timing_background();
  GCutG* get_gp_timing_prompt();

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
    if(!strcmp(cut_name, "aa_timing_background"))
    {
      aa_timing_background = cut;
    }
    if(!strcmp(cut_name, "aa_timing_prompt"))
    {
      aa_timing_prompt = cut;
    }
    if(!strcmp(cut_name, "gp_timing_prompt"))
    {
      gp_timing_prompt = cut;
    }
    if(!strcmp(cut_name, "gp_timing_background"))
    {
      gp_timing_background = cut;
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

  //Cutoff energy of the dE for an alpha
  const double dE_alpha_cutoff = 600.0; 

  //Check if there is no particle first, since this is relatively common. This will stop it from looping through unnecessarily
  if(dE_energy == -1)
  {
    return NO_PARTICLE;
  }

  TList *gates = &(obj.GetGates());
  int num_gates = gates->GetEntries();

  //Iterate through all gates
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
  
  /*Many alpha particles are stopped in the dE entirely, meaning no energy is deposited to the E. If that is the case, the incident alpha will not be plotted on the PID and
   will not be included in the gate. This if statement checks if the dE reaches some energy threshold, and if it does, it will be assumed that an alpha particle was observed.*/
  if(dE_energy > dE_alpha_cutoff)
  {
    return ALPHA;
  }
  else
  {
    return NO_PARTICLE;
  }

}


//GateInfo::get_x functions are used to read private variables of the GateInfo class
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

GCutG* GateInfo::get_aa_timing_background()
{
  return aa_timing_background;
}

GCutG* GateInfo::get_aa_timing_prompt()
{
  return aa_timing_prompt;
}

GCutG* GateInfo::get_gp_timing_prompt()
{
  return gp_timing_prompt;
}

GCutG* GateInfo::get_gp_timing_background()
{
  return gp_timing_background;
}

GCutG* GateInfo::get_gamma_dE_timing()
{
  return gamma_dE_timing;
}

//Return the event hit at some index. Hit is returned as an AddbackHit or as a regular hit depending on if global variable is_addback is true or not.
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

//Return the event size. Hit is returned as an AddbackSize or as a regular size depending on if global variable is_addback is true or not.
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

//Generates all general (non-gated, no gamma) histograms
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

/* Returns a string of the proper histogram directory name, depending on the number of dimensions of the histogram and the particle type. This is used to determine
   the folder in which a histogram will be placed inside of the .hist file generated. */
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

/*Makes a gamma-gamma timing spectrum, plotting gamma1.time-gamma2.time on the x axis and detector number on the y axis. Gammas will only make it into this histogram
  if one of the gammas was detected by detector n. */
void make_gamma_gamma_timing_if_det_n(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, char* directory, int det_num)
{
  
  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;
  
  int other_det_num;
  double gamma_dT;
  
  //Check if and which hit came from det n. If one came from det n, the timing will be calculated as hit_n.time - hit_other.time
  if(hit.GetNumber() == hit2.GetNumber())
  {
    return;
  }
  else if(hit.GetNumber() == det_num)
  {
    gamma_dT = hit.GetTime() - hit2.GetTime();
    other_det_num = hit2.GetNumber();
  } 
  else if(hit2.GetNumber() == det_num)
  {
    gamma_dT = hit2.GetTime() - hit.GetTime();
    other_det_num = hit.GetNumber();
  }
  else
  {
    return;
  }

  obj.FillHistogram(directory, Form("det_%d_%c%c_timing_mat", det_num, subscript, subscript), 200, -100, 100, gamma_dT,
                                                                              50, 0, 50, other_det_num);  

  
}

//Gamma gamma timing is plotted by taking the time difference of the higher energy vs the lower energy as the x axis, and the lower energy of the two hits as the y axis. 
void make_gamma_gamma_timing(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, double dT, double dT2, GateInfo* gates, char* directory)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;

  int gamma_dT;
  double lower_energy;
  int lower_energy_det_num;

  //Check which gamma is of the lower energy, then assign the time difference as the time of the higher energy minus the time of the lower energy.
  if(hit.GetEnergy() < hit2.GetEnergy())
  {
    lower_energy = hit.GetEnergy();
    lower_energy_det_num = hit.GetNumber();
    gamma_dT = hit2.GetTime() - hit.GetTime();
  }
  else
  {
    lower_energy = hit2.GetEnergy();
    lower_energy_det_num = hit2.GetNumber();
    gamma_dT = hit.GetTime() - hit2.GetTime();
  }

  obj.FillHistogram(directory, Form("%c%c_timing_mat", subscript, subscript), 2000, -1000, 1000, gamma_dT,
                                                                              6000, 0, 6000, lower_energy);
  obj.FillHistogram(directory, Form("%c%c_detector_timing_mat", subscript, subscript), 2000, -1000, 1000, gamma_dT,
                                                                              50, 0, 50, lower_energy_det_num);

  obj.FillHistogram(directory, Form("%c%c_detector_num_spectrum", subscript, subscript), 400, -200, 200, gamma_dT, 50, 0, 50, lower_energy_det_num);
  
}

//Make 1D histograms gated on time prompt or time random.
void make_1D_prompt_random(TRuntimeObjects &obj, TFSUHit* phit, double dT, GateInfo* gates, char* directory)
{

  TFSUHit hit = *phit;

  int particle_type = gates->get_particle_type();

  //boundries for square gates
  const double alpha_background_timing_low   = 10.0;
  const double alpha_background_timing_high  = 50.0;
  const double alpha_prompt_timing_low       = -40.0;
  const double alpha_prompt_timing_high      = 0.0; 

  const double proton_background_timing_low  = 20.0;
  const double proton_background_timing_high = 50.0;
  const double proton_prompt_timing_low      = -25.0;
  const double proton_prompt_timing_high     = 5.0;

  /* Check which type of particle coincided with the event. After, check if gamma-particle timing difference is within the bounds of the low and high of the prompt rectangle gate.
     If it is, populate the histogram, else, check if the gamma-particle timing difference is within the low and high background rectangle gate.*/
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
  
}


void fill_2D_histograms(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, double dT, double dT2, GateInfo* gates, TFSUHit* pdE)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;
  TFSUHit dE   = *pdE;

  char* directory = get_directory_name(2, gates);

  make_gamma_gamma_timing(obj, phit, phit2, dT, dT2, gates, directory);
  //Det 39 is used in make_gamma_gamma_timing_if_det_n due to high counts
  make_gamma_gamma_timing_if_det_n(obj, phit, phit2, directory, 39);
  obj.FillHistogramSym(directory, Form("%c%c_mat", subscript, subscript), 4000, 0, 4000, hit.GetEnergy(), 4000, 0, 4000, hit2.GetEnergy());

  obj.FillHistogram(directory, Form("particle_%c_timing_spectrum", subscript), 200,-100,100,(dE.GetTime() - hit.GetTime()), 2000,0,4000, hit.GetEnergy());
  obj.FillHistogram(directory, Form("particle_%c_detector_spectrum", subscript), 200,-100,100,(dE.GetTime() - hit.GetTime()), 50,0,50, hit.GetNumber());

}

//check if any 3 gamma's are time prompt. Between all pairs of 2 of the 3 hits passed in, the time of the higher energy is compared to the time of the lower energy gamma. If the resulting dT
//is not inside the prompt gate, the function will return false. If all combinations of gammas are within the gate, the function will return true.
bool is_ggg_time_prompt(TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3, GateInfo* gates)
{
  GCutG *prompt_gate;

  int dT;
  int lower_energy

  int   hits_size = 3
  TFSUHit* hits[] = {phit, phit2, phit3};

  if(subscript == 'g')
  {
    prompt_gate     = gates->get_gg_timing_prompt();
  }
  else if(subscript == 'a')
  {
    prompt_gate     = gates->get_aa_timing_prompt();
  }
  else
  {
    printf("Subscript not set in function fill_3D_gated_on_time_prompt_background!\n");
    exit(EXIT_FAILURE);
  }

  for(int i=0; i<hits_size; i++)
  {
    for(int j=i; j<hits_size; j++)
    {
      if(hits[i]->GetEnergy() > hits[j]->GetEnergy())
      {
	dT = hits[i]->GetTime() - hits[j]->GetTime();
	lower_energy = hits[j]->GetEnergy();

	if(!prompt_gate->IsInside(dT, lower_energy))
	{
	  return false;
	}
      }
      else
      {
	dT = hits[j]->GetTime() - hits[i]->GetTime();
	lower_energy = hits[i]->GetEnergy();

	if(!prompt_gate->IsInside(dT, lower_energy))
	{
	  return false;
	}
      }  
    }
  }
  return true;
}

bool is_ggg_time_background(TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3, GateInfo* gates)
{
  GCutG *background_gate;

  int dT;
  int lower_energy

  int   hits_size = 3
  TFSUHit* hits[] = {phit, phit2, phit3};

  if(subscript == 'g')
  {
    background_gate = gates->get_gg_timing_background();
  }
  else if(subscript == 'a')
  {
    background_gate = gates->get_aa_timing_background();
  }
  else
  {
    printf("Subscript not set in function fill_3D_gated_on_time_prompt_background!\n");
    exit(EXIT_FAILURE);
  }

  for(int i=0; i<hits_size; i++)
  {
    for(int j=i; j<hits_size; j++)
    {
      if(hits[i]->GetEnergy() > hits[j]->GetEnergy())
      {
	dT = hits[i]->GetTime() - hits[j]->GetTime();
	lower_energy = hits[j]->GetEnergy();

	if(!background_gate->IsInside(dT, lower_energy))
	{
	  return false;
	}
      }
      else
      {
	dT = hits[j]->GetTime() - hits[i]->GetTime();
	lower_energy = hits[i]->GetEnergy();

	if(!background_gate->IsInside(dT, lower_energy))
	{
	  return false;
	}
      }  
    }
  }
  return true;
}

void fill_3D_gated_on_time_prompt_background(TRuntimeObjects &obj, TFSUHit* pbase_hit, TFSUHit* phit, TFSUHit* phit2, int gate_energy, GateInfo* gates, char* directory)
{

  if(is_ggg_time_prompt(pbase_hit, phit1, phit2, gates))
  {
    obj.FillHistogramSym(directory, Form("%c%c%c_cube_prompt_%d", subscript, subscript, subscript, gate_energy), 4000, 0, 4000, phit->GetEnergy(), 4000, 0, 4000, phit2->GetEnergy());
  }

  if(background_gate->IsInside(dT, lower_energy))
  {
    obj.FillHistogramSym(directory, Form("%c%c%c_cube_background_%d", subscript, subscript, subscript, gate_energy), 4000, 0, 4000, phit->GetEnergy(), 4000, 0, 4000, phit2->GetEnergy());    
  }
}

/* General function for iterating over energies of interest when populating the 3D histograms. A 3D gamma-gamma-gamma histogram is generated in a similar manner to a 2D gamma-gamma histogram.
   Because plotting and working with a 3D histogram is difficult, 1 of the dimensions has an energy pre-selected so that we do not have to work with anything larger than a 2D histogram.
   This function iterates over all energies the user would like to pre-gate on (stored as an array of integers), checks if the 3rd hit (hit3) has an energy within some uncertainty of 
   that desired value, then will plot the other two hits (hit1 and hit2) if an only if the 3rd hit is within the bounds.*/
void iterate_over_energies_and_fill_3D(TRuntimeObjects &obj, TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3, double dT, double dT2, GateInfo* gates, int* energies_to_gate, int energies_to_gate_size, int uncertainty, char* directory)
{

  TFSUHit hit  = *phit;
  TFSUHit hit2 = *phit2;
  TFSUHit hit3 = *phit3;

  //Iterate over all energies of interest to gate on
  for(int i=0; i<energies_to_gate_size; i++)
  {
    int gate_energy = energies_to_gate[i];
    
    //Iterate over all 3 energies to fill symmetrically
    //fabs() is used to find the absolute difference in energy, so it can be compared to the uncertainty with one conditional
    if(fabs(hit.GetEnergy()-gate_energy) <= uncertainty)
    {
      obj.FillHistogramSym(directory, Form("%c%c%c_cube_%d", subscript, subscript, subscript, gate_energy), 4096, 0, 4096, hit2.GetEnergy(), 
			   4096, 0, 4096, hit3.GetEnergy()); 

      fill_3D_gated_on_time_prompt_background(obj, phit, phit2, phit3, gate_energy, gates, directory);
    }

    //fabs() is used to find the absolute difference in energy, so it can be compared to the uncertainty with one conditional
    else if(fabs(hit2.GetEnergy()-gate_energy) <= uncertainty)
    {
      obj.FillHistogramSym(directory, Form("%c%c%c_cube_%d", subscript, subscript, subscript, gate_energy), 4096, 0, 4096, hit.GetEnergy(), 
			   4096, 0, 4096, hit3.GetEnergy()); 

      fill_3D_gated_on_time_prompt_background(obj, phit2, phit, phit3, gate_energy, gates, directory);
    }

    //fabs() is used to find the absolute difference in energy, so it can be compared to the uncertainty with one conditional
    else if(fabs(hit3.GetEnergy()-gate_energy) <= uncertainty)
    {
      obj.FillHistogramSym(directory, Form("%c%c%c_cube_%d", subscript, subscript, subscript, gate_energy), 4096, 0, 4096, hit.GetEnergy(), 
			   4096, 0, 4096, hit2.GetEnergy()); 

      fill_3D_gated_on_time_prompt_background(obj, phit3, phit, phit2, gate_energy, gates, directory);
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

  //Uncertainty determined the energy range in which a gamma ray will be considered (e.g. an uncertainty of 2 will consider allow a 1552 to be considered as a 1554)
  int uncertainty = 2;

  //energies_to_gate is an array that contains the centroids of interest of a cube coincidence. 
  int sc_47_energies_to_gate_size = 2;
  int sc_47_energies_to_gate[] = {1001, 1147};
  char sc_47_3D_dir[] = "sc_47_3D"; 
  iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, sc_47_energies_to_gate, sc_47_energies_to_gate_size, uncertainty, sc_47_3D_dir);

  //int sc_48_energies_to_gate_size = 2;
  //int sc_48_energies_to_gate[] = {370, 520};
  //char sc_48_3D_dir[] = "sc_48_3D";
  //iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, sc_48_energies_to_gate, sc_48_energies_to_gate_size, uncertainty, "sc_48_3D_dir");

  //int ca_44_energies_to_gate_size = 4;  
  //int ca_44_energies_to_gate[] = {1157, 726, 1883, 1126};
  //char ca_44_3D_dir[] = "ca_44_3D";
  //iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, ca_44_energies_to_gate, ca_44_energies_to_gate_size, uncertainty, "ca_44_3D_dir");

  //int ca_45_energies_to_gate_size = 3;
  //int ca_45_energies_to_gate[] = {1554, 1324, 1064};
  //char ca_45_3D_dir[] = "ca_45_3D";
  //iterate_over_energies_and_fill_3D(obj, phit, phit2, phit3, dT, dT2, gates, ca_45_energies_to_gate, ca_45_energies_to_gate_size, uncertainty, ca_45_3D_dir);

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

bool hits_within_energy_bounds(TFSUHit* phit)
{
  
  double energy1 = phit->GetEnergy();

  if(energy1 > energy_low_cutoff && energy1 < energy_high_cutoff)
  {
    return true;
  }
  
  return false;
}

bool hits_within_energy_bounds(TFSUHit* phit, TFSUHit* phit2)
{
  
  double energy1 = phit->GetEnergy();
  double energy2 = phit2->GetEnergy();

  if(energy1 < energy_low_cutoff || energy1 > energy_high_cutoff)
  {
    return false;
  }
  if(energy2 < energy_low_cutoff || energy2 > energy_high_cutoff)
  {
    return false;
  }

  return true;
}

bool hits_within_energy_bounds(TFSUHit* phit, TFSUHit* phit2, TFSUHit* phit3)
{
  
  double energy1 = phit->GetEnergy();
  double energy2 = phit2->GetEnergy();
  double energy3 = phit3->GetEnergy();

  if(energy1 < energy_low_cutoff || energy1 > energy_high_cutoff)
  {
    return false;
  }
  if(energy2 < energy_low_cutoff || energy2 > energy_high_cutoff)
  {
    return false;
  }
  if(energy3 < energy_low_cutoff || energy3 > energy_high_cutoff)
  {
    return false;
  }

  return true;
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


  for(unsigned int i=0; i<size; i++) 
  {

    //Get the hit, find the timing difference between dE and the gamma, the populate
    TFSUHit hit  = get_event_hit(fsu_event, is_addback, i);
    double dT = dE.GetTime() - hit.GetTime();
    
    
    if(hits_within_energy_bounds(&hit))
    {
      fill_1D_histograms(obj, &hit, dT, &dE, &gates);
    }

    //Check if there are atleast 2 hits for 2D analysis
    if(size >= 2)
    {
      for(unsigned int j=i+1; j < size; j++)
      {

	//Get the hit, find the timing difference between dE and the gamma, the populate
	TFSUHit hit2 = get_event_hit(fsu_event, is_addback, j);
	double dT2 = dE.GetTime() - hit2.GetTime();

	
	if(hits_within_energy_bounds(&hit, &hit2))
	{
	  fill_2D_histograms(obj, &hit, &hit2, dT, dT2, &gates, &dE);
	}
	if(size >= 3)
	{
	  for(unsigned int k=j+1; k<size; k++)
	  {
	    
	    TFSUHit hit3 = get_event_hit(fsu_event, is_addback, k);

	    if(hits_within_energy_bounds(&hit, &hit2, &hit3))
	    {
	      fill_3D_histograms(obj, &hit, &hit2, &hit3, dT, dT2, &gates);
	    }
	  }
	}
      }
    }
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
