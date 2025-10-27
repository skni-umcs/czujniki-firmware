#ifndef PASSTHROUGHCOMMUNICATION_H
#define PASSTHROUGHCOMMUNICATION_H

#include <queue>

#include "small_communication.h"
#include "time/time_slot_manager.h"
#include "time/waiter.h"

class PassthroughCommunication : public SmallCommunication {
  std::vector<std::shared_ptr<LoraMessage>> messageSet;
  std::shared_ptr<Waiter> sendWaiter = Waiter::create();
  ;
  bool isOldLoopActive = false;
  std::shared_ptr<TimeSlotManager> timeSlotManager;

  TaskHandle_t passthroughTaskHandle = nullptr;
  std::shared_ptr<LoraMessage> scheduledMessage = nullptr;
  TickType_t scheduledDelayTicks = 0;
  static void passthroughTaskFunc(void* pvParameters);

 protected:
  PassthroughCommunication() {};

 public:
  static std::shared_ptr<PassthroughCommunication> create();
  OperationResult getNotified(std::shared_ptr<Message> message) override;
  std::vector<std::shared_ptr<LoraMessage>> getSameMessages(
      std::shared_ptr<LoraMessage> message);
  OperationResult updateSetFromNewMessage(std::shared_ptr<Message> message);
  OperationResult removeSameMessages(
      std::vector<std::shared_ptr<LoraMessage>>& rebroadcastedMessages,
      std::shared_ptr<LoraMessage> message);
  OperationResult rebroadcast(std::shared_ptr<LoraMessage> message);
  OperationResult transmit(std::shared_ptr<Message> message) override;
  bool shouldRebroadcast(std::shared_ptr<LoraMessage> message);
  OperationResult rebroadcastAfterWait(
      std::shared_ptr<LoraMessage> loraMessage);
  OperationResult ponderAfterWait(bool isLoop);
  OperationResult processNewMessage();
  int getMessageSetLength();
  bool getIsSendWaiting();
  void setTimeSlotManager(std::shared_ptr<TimeSlotManager> manager);
};

#endif