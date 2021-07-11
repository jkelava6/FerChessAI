#pragma once

#include <Threading/ThreadingUtils.h>


template<class FPopInfo, class FUnitInfo> static void ThreadedMatch(void* VoidMatchPtr)
{
	TMatchData<FPopInfo, FUnitInfo>& Match = *(TMatchData<FPopInfo, FUnitInfo>*)VoidMatchPtr;
	Match.Play();
}

TMatchData<class FPopInfo, class FUnitInfo>::TMatchData(TLeagueApi<FPopInfo, FUnitInfo>* InLeague) :
	League(InLeague)
{
	if (League)
	{
		League->Internal_PlayMatch(*this);
	}
}

template<class FPopInfo, class FUnitInfo> TLeagueApi<FPopInfo, FUnitInfo>::TLeagueApi()
{
	Clear();
}

template<class FPopInfo, class FUnitInfo> TLeagueApi<FPopInfo, FUnitInfo>::~TLeagueApi() = default;

template<class FPopInfo, class FUnitInfo> TLeagueApi<FPopInfo, FUnitInfo>& TLeagueApi<FPopInfo, FUnitInfo>::operator=(const TLeagueApi<FPopInfo, FUnitInfo>& Copied)
{
	Base = Copied.Base;
	PopCount = Copied.PopCount;
	PopSize = Copied.PopSize;
	Generation = Copied.Generation;

	DataDna = Copied.DataDna;
	DataPops = Copied.DataPops;
	DataUnits = Copied.DataUnits;
}

template<class FPopInfo, class FUnitInfo> TLeagueApi<FPopInfo, FUnitInfo>::TLeagueApi(const TLeagueApi<FPopInfo, FUnitInfo>& Copied)
{
	*this = Copied;
}

template<class FPopInfo, class FUnitInfo> TLeagueApi<FPopInfo, FUnitInfo>& TLeagueApi<FPopInfo, FUnitInfo>::operator=(TLeagueApi<FPopInfo, FUnitInfo>&& Moved)
{
	Base = Moved.Base;
	PopCount = Moved.PopCount;
	PopSize = Moved.PopSize;
	Generation = Moved.Generation;
	Base->SetLeague(*this);

	DataDna = Move(Moved.DataDna);
	DataPops = Move(Moved.DataPops);
	DataUnits = Move(Moved.DataUnits);

	Moved.Clear();
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::Init(ILeagueBase<FPopInfo, FUnitInfo>* InBase, int PopCount, int PopSize)
{
	// TODO: implement init function
}

template<class FPopInfo, class FUnitInfo> FDna& TLeagueApi<FPopInfo, FUnitInfo>::AccessDna(int PopIndex, int UnitIndex)
{
	return DataDna[PopIndex * PopSize + UnitIndex];
}

template<class FPopInfo, class FUnitInfo> const FDna& TLeagueApi<FPopInfo, FUnitInfo>::AccessDna(int PopIndex, int UnitIndex) const
{
	return DataDna[PopIndex * PopSize + UnitIndex];
}

template<class FPopInfo, class FUnitInfo> FPopInfo& TLeagueApi<FPopInfo, FUnitInfo>::AccessPopInfo(int PopIndex)
{
	return DataPops[PopIndex];
}

template<class FPopInfo, class FUnitInfo> const FPopInfo& TLeagueApi<FPopInfo, FUnitInfo>::AccessPopInfo(int PopIndex) const
{
	return DataPops[PopIndex];
}

template<class FPopInfo, class FUnitInfo> FUnitInfo& TLeagueApi<FPopInfo, FUnitInfo>::AccessUnitInfo(int PopIndex, int UnitIndex)
{
	return DataUnits[PopIndex * PopSize + UnitIndex];
}

template<class FPopInfo, class FUnitInfo> const FUnitInfo& TLeagueApi<FPopInfo, FUnitInfo>::AccessUnitInfo(int PopIndex, int UnitIndex) const
{
	return DataUnits[PopIndex * PopSize + UnitIndex];
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::Clear()
{
	Base = nullptr;
	PopCount = 0;
	PopSize = 0;
	Generation = 0;
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::IterateFor(int GenerationCount)
{
	for (int Iteration = 0; Iteration < GenerationCount; ++Iteration)
	{
		IterateSingle();
	}
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::IterateTo(int FinalGeneration)
{
	if (FinalGeneration > Generation)
	{
		IterateFor(FinalGeneration - Generation);
	}
}

template<class FPopInfo, class FUnitInfo> int TLeagueApi<FPopInfo, FUnitInfo>::GetGeneration() const
{
	return Generation;
}

template<class FPopInfo, class FUnitInfo> int TLeagueApi<FPopInfo, FUnitInfo>::GetPopCount() const
{
	return PopCount;
}

template<class FPopInfo, class FUnitInfo> int TLeagueApi<FPopInfo, FUnitInfo>::GetPopSize() const
{
	return PopSize;
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::CreateThreadPool(int ThreadCount)
{
	AssignThreadPool(AiThreads::InitializeThreadPool(ThreadCount));
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::AssignThreadPool(int PoolId)
{
	AssignedThreadPool = PoolId;
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::SetThreaded(bool bThreaded)
{
	bPlayThreaded = bThreaded;
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::IterateSingle()
{
	Base->PreIteration();

	TArray<TMatchData<FPopInfo, FUnitInfo>> Matches;
	Base->CreateMatches(Matches);

	if (bPlayThreaded && AssignedThreadPool == -1)
	{
		CreateThreadPool(PreferedThreadCount);
	}

	const int MatchCount = Matches.Count();
	for (int MatchId = 0; MatchId < MatchCount; ++MatchId)
	{
		if (bPlayThreaded)
		{
			AiThreads::QueueTaskBlocking(AssignedThreadPool, ThreadedMatch<FPopInfo, FUnitInfo>, &Matches[MatchId]);
		}
		else
		{
			Internal_PlayMatch(Matches[MatchId]);
		}
	}

	for (int MatchId = 0; MatchId < MatchCount; ++MatchId)
	{
		TArray<FMatchParticipant> Participants = Matches[MatchId].Participants;
		for (int ParticipantIndex = 0; ParticipantIndex < Participants.Count(); ++ParticipantIndex)
		{
			if (Participants[ParticipantIndex].bRated)
			{
				Base->GradeAI(Matches[MatchId], Participants[ParticipantIndex].PopIndex, Participants[ParticipantIndex].UnitIndex);
			}
		}
	}

	Base->PostIteration();

	++Generation;
}

template<class FPopInfo, class FUnitInfo> void TLeagueApi<FPopInfo, FUnitInfo>::Internal_PlayMatch(TMatchData<FPopInfo, FUnitInfo>& Match)
{
	Base->PlayMatch(Match);
}
