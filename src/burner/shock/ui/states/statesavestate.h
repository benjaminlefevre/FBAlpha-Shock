
// See License.md for license

#ifndef STATESAVESTATE_H_
#define STATESAVESTATE_H_

#include "shock/ui/states/stategamestatebase.h"

class StateSaveState : public StateGameStateBase
{
public:
    virtual void Create( );
    virtual void Destroy( );

    virtual void EnterState( UIState oldState );
    virtual void ExitState( UIState newState );

    virtual UIState Update( );

protected:
    virtual void DrawMenu( );
};

#endif
