#include <sipstack/TransactionState.hxx>
#include <sipstack/SipStack.hxx>
#include <sipstack/SipMessage.hxx>
#include <sipstack/TimerMessage.hxx>

using namespace Vocal2;

void
Vocal2::TransactionState::process(SipStack& stack)
{
   Message* message = stack.mStateMacFifo.getNext();
   
   SipMessage* sip = dynamic_cast<SipMessage*>(message);
   TimerMessage* timer=0;
   
   if (sip == 0)
   {
      timer = dynamic_cast<TimerMessage*>(message);
   }
   
   Data& tid = message->getTransactionId();
   TransactionState* state = stack.mTransactionMap.find(tid);
   if (state) // found transaction for sip msg
   {
      switch (state->mMachine)
      {
         case ClientNonInvite:
            processClientNonInvite(message);
            break;
         case ClientInvite:
            processClientInvite(message);
            break;
         case ServerNonInvite:
            processServerNonInvite(message);
            break;
         case ServerInvite:
            processClientInvite(message);
            break;
         case Stale:
            processStale(message);
            break;
         default:
            assert(0);
      }
   }
   else // new transaction
   {
      if (sip)
      {
         if (sip->isRequest())
         {
            // create a new state object and insert in the TransactionMap
               
            if (sip->isExternal()) // new sip msg from transport
            {
               if (sip[RequestLine].getMethod() == INVITE)
               {
                  TransactionState* state = new TransactionState(ServerInvite, Proceeding);
                  stack.mTimers.add(Timer::TimerTrying, tid, Timer::T100)
                     stack.mTransactionMap.add(tid,state);
               }
               else 
               {
                  TransactionState* state = new TransactionState(ServerNonInvite,Trying);
                  stack.mTransactionMap.add(tid,state);
               }
               stack.mTUFifo.add(sip);
            }
            else // new sip msg from the TU
            {
               if (sip[RequestLine].getMethod() == INVITE)
               {
                  TransactionState* state = new TransactionState(ClientInvite, Calling);
                  stack.mTimers.add(Timer::TimerB, tid, 64*Timer::T1 );
                  stack.mTransactionMap.add(tid,state);
               }
               else 
               {
                  TransactionState* state = new TransactionState(ClientNonInvite, Trying);
                  stack.mTimers.add(Timer::TimerF, tid, 64*Timer::T1 );
                  stack.mTransactionMap.add(tid,state);
               }
               stack.mTransportSelector.send(sip);
            }
         }
         else if (sip->isResponse()) // stray response
         {
            if (stack.mDiscardStrayResponses)
            {
               DebugLog (<< "discarding stray response: " << sip->brief());
               delete message;
            }
            else
            {
               // forward this statelessly
               assert(0);
            }
         }
         else // wasn't a request or a response
         {
            DebugLog (<< "discarding unknown message: " << sip->brief());
         }
      } 
      else // timer or other non-sip msg
      {
         DebugLog (<< "discarding non-sip message: " << message->brief());
         delete message;
      }
   }
}

void
TransactionState::processClientNonInvite(  Message* msg )
{ 

}


void
TransactionState::processClientInvite(  Message* msg )
{
}


void
TransactionState::processServerNonInvite(  Message* msg )
{
   SipMessage* sip = dynamic_cast<SipMessage*>(message);
   TimerMessage* timer=0;
   if (sip == 0)
   {
      timer = dynamic_cast<TimerMessage*>(message);
   }
   
   if (timer)
   {
      switch ( timer.getType() )
      {
         case TimerJ:
         {
            mState = Terminated;
         }
         break;
      }
   }

   if (sip)
   {
      assert(0);
      
      if (0)
      {
         // its a response 
         responseCode = 0; assert(0); // TODO 

         if (responseCode<200)
         {
            mState = Proceeding;
         }
         else
         { 
            mState = Completed;
         }
      }
      else
      {
         // it is a request
       }
   }
}


void
TransactionState::processServerInvite(  Message* msg )
{
}


void
TransactionState::processStale(  Message* msg )
{
}
