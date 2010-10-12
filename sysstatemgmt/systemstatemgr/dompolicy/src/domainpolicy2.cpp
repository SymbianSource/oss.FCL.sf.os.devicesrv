// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "ssmdomaindefs.h"
#include "ssmsubstates.hrh"
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
#include "ssmstates.hrh"
#endif //SYMBIAN_INCLUDE_APP_CENTRIC

#ifdef __WINS__
const TInt KStateTransitionTimeout = 30000000; /* 30 seconds */
#else
const TInt KStateTransitionTimeout = 10000000; /* 10 seconds */
#endif

#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
const TInt KStateDeferredLimit = 1; /* 1 time */
const TInt KMemberTimeout = 5; /* 5msec */
#endif //SYMBIAN_INCLUDE_APP_CENTRIC
/*
Domain specification and policy for the startup domain hierarchy
*/

static const TDmDomainSpec DomainHierarchy[] =
	{
		{ KSM2HALDomain3,				KDmIdNone,					_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2KernelServicesDomain3,	KSM2HALDomain3,				_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2OSServicesDomain3,		KSM2KernelServicesDomain3,	_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout},
		{ KSM2GenMiddlewareDomain3,		KSM2OSServicesDomain3,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined,	KStateTransitionTimeout },
		{ KSM2GenMiddlewareDomain4,		KSM2GenMiddlewareDomain3,	_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined,	KStateTransitionTimeout },
		{ KSM2AppServicesDomain3,		KSM2GenMiddlewareDomain4,	_INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2AppServicesDomain4,		KSM2AppServicesDomain3,	    _INIT_SECURITY_POLICY_PASS, ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2UiServicesDomain3,		KSM2AppServicesDomain4,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		{ KSM2UiApplicationDomain3,		KSM2UiServicesDomain3,		_INIT_SECURITY_POLICY_PASS,	ESsmStartupSubStateUndefined, 	KStateTransitionTimeout },
		// end of array marker
		TDM_DOMAIN_SPEC_END
	};

#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
/**
 * Note that the transition monitor feature is enabled for states added to StateSpecification array only.
 */
static const SDmStateSpecV1 StateSpecification[]=
    {
        {(ESsmShutdown << 16 | ESsmShutdownSubStateCritical), KMemberTimeout, KStateDeferredLimit, ETransitionFailureUsePolicyFromOrdinal3},
        {(ESsmShutdown << 16 | ESsmShutdownSubStateNonCritical), KMemberTimeout, KStateDeferredLimit, ETransitionFailureUsePolicyFromOrdinal3}
           
    };


#endif //SYMBIAN_INCLUDE_APP_CENTRIC
/*
Note that the _INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData) is not used for the root domain, since this refers to the capabilities of
the SSA components connecting to the Start-up Domain Hierarchy. No capabilities are required by SSA components to attach to the Start-up Domain Hierarchy.
*/

// Policy for traversal of the startup Domain Hierarchy. Only positive transitions should take place
static const TDmHierarchyPolicy HierarchyPolicyStartup	=
	{ETraverseParentsFirst, ETraverseParentsFirst, ETransitionFailureContinue};

/**
Gets access to the domain hierarchy specification.

The domain hierarchy specification is a simple array of TDmDomainSpec items.

The default implementation provided by Symbian OS just returns a pointer to
the domain hierarchy specification array.

@return A pointer to the domain hierarchy specification array.
*/
EXPORT_C const TDmDomainSpec* DmPolicy::GetDomainSpecs()
	{
	return (TDmDomainSpec*) DomainHierarchy;
	}




/**
Releases access to the specified domain hierarchy specification.

The domain hierarchy specification is a simple array of TDmDomainSpec items.

As the default Symbian OS implementation of GetDomainSpecs() just returns
a pointer to the domain hierarchy specification array, then the default
implementation of Release() is empty. The API is provided to permit
more complex implementations, if required.

@param aDomainSpec A pointer to the domain hierarchy specification array.
*/
EXPORT_C void DmPolicy::Release(const TDmDomainSpec* /*aDomainSpec*/)
	{
	}


/**
Retrieves the domain hierarchy policy.

@param	aPolicy a client-supplied policy which on exit
		will contain a copy of the policy for the requested domain hierarchy Id.

@return	KErrNone
*/
EXPORT_C TInt DmPolicy::GetPolicy(TDmHierarchyPolicy& aPolicy)
	{
	aPolicy = HierarchyPolicyStartup;
	return KErrNone;
	}

/**
Retrieves the state specification array.
The domain manager retrieves the StateSpecification array to read the state for which the transition
monitoring feature has to be enabled.
This should be present in the oridinal4 of the domainpolicy def file.

@param aPtr  Will have the state specification structure used in the array .
@param aNumElements Will hold the number of elements in the array.

@return will return the version of the domain policy if the feature is enabled or else returns KErrNone.
*/
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
EXPORT_C  TInt DmPolicy::GetStateSpec(TAny*& aPtr, TUint& aNumElements)
    {    
    aPtr = (TAny*) StateSpecification;
    aNumElements = sizeof(StateSpecification)/sizeof(SDmStateSpecV1);    
    return KSDmStateSpecV1;    
    }
#else
EXPORT_C  TInt DmPolicy::GetStateSpec(TAny*& aPtr, TUint& aNumElements)
    {
    aPtr = NULL;
    aNumElements = 0;
    return KErrNone;  
    }
#endif //SYMBIAN_INCLUDE_APP_CENTRIC

/**
Defines the function type for a static function that is implemented by
a device's domain policy DLL at ordinal 5. 
The domain manager uses this function to release the state specification returned by ordinal 4. 
The implementation may be empty and simply return if no release action needs 
to be taken.
 */
EXPORT_C void DmPolicy::ReleaseStateSpec(TAny* /*aStateSpec*/)
    {
    }

