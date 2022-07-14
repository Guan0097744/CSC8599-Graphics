#ifndef STATES_HPP
#define STATES_HPP

// Namespace to tie together all states functions
namespace States 
{

    //============================================================================================//
    //Check state
    //For example: states = 10001101
    //============================================================================================//

    template<typename T>
    bool IsIndexActive(T* states, int target) 
    {
        /*
            target = 3
            1 = 00000001
            1 << target = 00001000

            10001101
            00001000 &
            --------
            00001000 == 1 << target
        */

        return (*states & (1 << target));
    }

    // activate state
    template<typename T>
    void ActivateIndex(T* states, int target) 
    {
        /*
            target = 4
            1 = 00000001
            1 << target = 00010000

            10001101
            00010000 |
            --------
            10011101
        */

        * states |= 1 << target;
    }

    // uniquely activate state (no others can be active)
    template<typename T>
    void UniquelyActivateIndex(T* states, int target) 
    {
        /*
            target = 4
            1 = 00000001
            1 << target = 00010000

            10000000 (AS)
            00010000 |
            --------
            10010000
            00010000 &
            --------
            00010000
        */

        ActivateIndex<T>(states, target); // activate state first
        *states &= 1 << target; // deactivate others
    }

    // deactivate states
    template<typename T>
    void DeactivateIndex(T* states, int target) 
    {
        /*
            target = 4
            1 = 00000001
            1 << target = 00010000
            ~(1 << target) = 11101111

            10011101
            11101111 &
            --------
            10001101
        */

        * states &= ~(1 << target);
    }

    // toggle state
    template<typename T>
    void ToggleIndex(T* states, int target) 
    {
        /*
            target = 4
            1 = 00000001
            1 << target = 00010000

            10001101
            00010000 ^
            --------
            10011101
        */

        * states ^= 1 << target;
    }

    // tell if state is active
    template<typename T>
    bool IsActive(T* states, T state) 
    {
        return (*states & state);
    }

    // activate state
    template<typename T>
    void Activate(T* states, T state) 
    {
        *states |= state;
    }

    // uniquely activate state (no others can be active)
    template<typename T>
    void UniquelyActivate(T* states, T state) 
    {
        *states &= state;
    }

    // deactivate state
    template<typename T>
    void Deactivate(T* states, T state) 
    {
        *states &= ~state;
    }

    // toggle state
    template<typename T>
    void Toggle(T* states, T state) 
    {
        *states ^= state;
    }
}

#endif