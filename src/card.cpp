#include "card.hpp"
#include "debug.h"

#include <fstream>

/////////////////////////////////
// Constructor
/////////////////////////////////

Card::Card(){}

/////////////////////////////////
// Private methods
/////////////////////////////////

std::string Card::completeBlock(std::string  const& word_p,
                                   unsigned int        blockSize_p
                                  )
{ 
    unsigned int size = blockSize_p - word_p.size();
    if(size == 0){
        Log("Error with "+word_p+" block in datacard");
        return "";
    }
    std::string spaces;
    for(size_t i = 0; i < size; ++i){
        spaces += " ";
    }
    return word_p+spaces;
}

/////////////////////////////////
// Public methods
/////////////////////////////////

void Card::addSeparator()
{
    datacard += "--------------------------------------------------------------------------------------------- \n";
}


void Card::addGlobalParameter(namelist const& groupList_p,
                              int             numberOfBins)
{
    datacard += "imax "+std::to_string(numberOfBins)+" number of bins\n";
    datacard += "jmax "+std::to_string(groupList_p.size()-1)+" number of background processes\n";
    datacard += "kmax * number of nuisance parameters\n";
}

void Card::addInputsProcess(std::string const& rootfile_p)
{
    datacard += "shapes * * ./inputs/"+rootfile_p+" $PROCESS $PROCESS_$SYSTEMATIC\n";
    datacard += "shapes sig * ./inputs/"+rootfile_p+" $PROCESS $PROCESS_$SYSTEMATIC\n";
}

void Card::addChanels(std::string const& observable_p,
                      double             numberOfEvents_p
                     )
{
    datacard += "bin "+observable_p+"\n";
    datacard += "observation "+std::to_string(numberOfEvents_p)+"\n";
}


void Card::addProcToCard(std::string const& observable_p,
                         namelist    const& groupList_p
                        )
{
    unsigned int block_proc = 30;
    unsigned int block_grp  = 15;

    std::string line0 = completeBlock("bin", block_proc);
    std::string line1 = completeBlock("process", block_proc);
    std::string line2 = line1;
    std::string line3 = completeBlock("rate", block_proc);

    for(size_t i = 0; i < groupList_p.size(); ++i){
        line0 += completeBlock(observable_p, block_grp);
        line1 += completeBlock(groupList_p[i], block_grp);
        line2 += completeBlock(std::to_string(i), block_grp);
        line3 += completeBlock("-1", block_grp);
    }
    datacard += line0+'\n'+line1+'\n'+line2+'\n'+line3+'\n'; 
}

void Card::addSystToCard(std::string const& systName_p,
                            std::string const& shape_p,
                            namelist    const& groupList_p,
                            std::string const& value_p
                           )
{
    datacard += completeBlock(systName_p, 24) + completeBlock(shape_p, 6);
    for(size_t i = 0; i < groupList_p.size(); ++i)
        datacard += completeBlock(value_p, 12);
    datacard += '\n';
}

void Card::addRateToCard(namelist    const& groupList_p,
                         namelist    const& systematicsRate_p
                        )
{
    for(size_t i = 1; i < groupList_p.size(); ++i){
        std::string line = completeBlock("r"+groupList_p[i], 24) + completeBlock("lnN", 6);
        for(size_t j = 0; j < groupList_p.size(); ++j){
            if(i == j)
                line += completeBlock(systematicsRate_p[j-1], 12);
            else
                line += completeBlock("0", 12);
        }  
        datacard += line + '\n';
    }
}

void Card::printCard()
{
    std::cout << " >> datacard :" << std::endl;
    std::cout << datacard << std::endl;
}

void Card::saveCard(std::string const& name_p)
{
    std::ofstream file(name_p);
    file << datacard;
    file.close();
}
