#ifndef TIME_SLOT_MANAGER_H
#define TIME_SLOT_MANAGER_H

#include <utils/storage_types.h>

#include "time_constants.h"

class TimeSlotManager {
 private:
  static const int SLOT_DURATION_MS = 5000;  // 5 sekund na slot
  static const int SLOTS_PER_CYCLE = 100;    // 100 slotów = ~8.20 min cykl

  moduleAddress myAddress;
  unsigned long lastSyncEpoch = 0;
  bool synchronized = false;

 public:
  TimeSlotManager(moduleAddress address) : myAddress(address) {}

  void markTimeSynchronized(unsigned long serverEpoch) {
    synchronized = true;
    lastSyncEpoch = serverEpoch;
  }

  bool isTimeSynchronized() const { return synchronized; }

  int getMySlotNumber() const { return myAddress % SLOTS_PER_CYCLE; }

  unsigned long getDelayToNextSlot(unsigned long currentEpoch) const {
    if (!synchronized) {
      return 0;
    }

    unsigned long timeSinceSync =
        (currentEpoch - lastSyncEpoch) * 1000;  // na ms
    unsigned long cycleTime = SLOT_DURATION_MS * SLOTS_PER_CYCLE;
    unsigned long positionInCycle = timeSinceSync % cycleTime;

    int mySlot = getMySlotNumber();
    unsigned long mySlotStartMs = mySlot * SLOT_DURATION_MS;

    if (positionInCycle < mySlotStartMs) {
      return mySlotStartMs - positionInCycle;
    } else {
      return cycleTime - positionInCycle + mySlotStartMs;
    }
  }

  bool isMySlotActive(unsigned long currentEpoch) const {
    if (!synchronized) {
      return true;  // Bez synchronizacji zawsze pozwalamy
    }

    unsigned long delayMs = getDelayToNextSlot(currentEpoch);
    // Slot aktywny jeśli opóźnienie < SLOT_DURATION_MS (jesteśmy w naszym
    // slocie)
    return delayMs >= (SLOT_DURATION_MS * (SLOTS_PER_CYCLE - 1));
  }

  int getSlotDurationMs() const { return SLOT_DURATION_MS; }

  int getSlotsPerCycle() const { return SLOTS_PER_CYCLE; }
};

#endif
