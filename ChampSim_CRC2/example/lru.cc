#include "../inc/champsim_crc2.h"

#define NUM_CORE 1
#define LLC_SETS NUM_CORE*2048
#define LLC_WAYS 16

uint32_t lru[LLC_SETS][LLC_WAYS];
uint32_t bip[LLC_SETS][LLC_WAYS];

bool use_LRU= false; // I just have this set to false and only use BIP, but I'll keep the LRU implementation here since it's interesting

// initialize replacement state
void InitReplacementState()
{
    cout << "Initialize LRU replacement state" << endl;

    for (int i=0; i<LLC_SETS; i++) {
        for (int j=0; j<LLC_WAYS; j++) {
            lru[i][j] = j;
            bip[i][j] = j;
        }
    }
}

// find replacement victim
// return value should be 0 ~ 15 or 16 (bypass)
uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    int lru_return_value = 0;
    bool lru_return_value_set = false;
    int bip_return_value = 0;
    bool bip_return_value_set = false;

    for (int i=0; i<LLC_WAYS; i++) {
        if (lru[set][i] == (LLC_WAYS-1) && !lru_return_value_set) {
            lru_return_value = i;
            lru_return_value_set = true;
        }   
        if (bip[set][i] == (LLC_WAYS-1) && !bip_return_value_set) {
            bip_return_value = i;
            bip_return_value_set = true;
        }
    }

    if (use_LRU) { // supposed to keep track of both return values if the dynamic switching was more effective
        return lru_return_value;
    }      
    else {
        return bip_return_value;
    }

    return 0;
}

// called on every cache hit and cache fill
void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    // update lru replacement state
    for (uint32_t i=0; i<LLC_WAYS; i++) {
        if (lru[set][i] < lru[set][way]) {
            lru[set][i]++;

            if (lru[set][i] == LLC_WAYS)
                assert(0);
        }
        if (bip[set][i] < bip[set][way]) {
            bip[set][i]++;

            if (bip[set][i] == LLC_WAYS)
                assert(0);
        }
    }

    if (hit) {
        lru[set][way] = 0; // promote to MRU position
    }
    else {
        lru[set][way] = 8; // after some experimentation, LRU status of 8 had the least cache misses
    }
    if (rand()%2048 == 0) { // after some experimentation, doing %2048 yielded the least cache misses
        bip[set][way] = 0; // promote to the MRU position
    }
    else {
        bip[set][way] = 8; // after some experimentation, LRU status of 8 had the least cache misses
    }
}

// use this function to print out your own stats on every heartbeat 
void PrintStats_Heartbeat()
{

}

// use this function to print out your own stats at the end of simulation
void PrintStats()
{

}
