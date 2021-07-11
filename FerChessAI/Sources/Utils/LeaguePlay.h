#pragma once

#include <Array.h>
#include <BuildFlags.h>

class FDna;
template <class FPopInfo, class FUnitInfo> class TLeagueApi;

class FMatchParticipant
{
public:
	FMatchParticipant() = default;
	FMatchParticipant(int InPopIndex, int InUnitIndex, bool bInRated) :
		PopIndex(InPopIndex), UnitIndex(InUnitIndex), bRated(bInRated) {};
public:
	int PopIndex = -1;
	int UnitIndex = -1;
	bool bRated = false;
	void* Data = nullptr;
};

template <class FPopInfo, class FUnitInfo> class TMatchData
{
public:
	TMatchData() = default;
	TMatchData(TLeagueApi<FPopInfo, FUnitInfo>* InLeague);
	void Play();
public:
	TArray<FMatchParticipant> Participants;
	TLeagueApi<FPopInfo, FUnitInfo>* League = nullptr;
};

template <class FPopInfo, class FUnitInfo> class ILeagueBase
{
public:
	virtual TLeagueApi<FPopInfo, FUnitInfo>& GetLeague() = 0;
	virtual void SetLeague(TLeagueApi<FPopInfo, FUnitInfo>& League) = 0;

	virtual void GenerateInitialDna() = 0;
	virtual void PreIteration() = 0;
	virtual void CreateMatches(TArray<TMatchData<FPopInfo, FUnitInfo>> List) = 0;
	virtual void PlayMatch(TMatchData<FPopInfo, FUnitInfo>& Match) = 0;
	virtual void GradeAI(TMatchData<FPopInfo, FUnitInfo>& Match, int PopIndex, int UnitIndex) = 0;
	virtual void PostIteration() = 0;
};

template <class FPopInfo, class FUnitInfo> class TLeagueApi
{
public:
	TLeagueApi();
	~TLeagueApi();
	TLeagueApi& operator=(const TLeagueApi<FPopInfo, FUnitInfo>& Copied);
	TLeagueApi(const TLeagueApi<FPopInfo, FUnitInfo>& Copied);
	TLeagueApi& operator=(TLeagueApi<FPopInfo, FUnitInfo>&& Moved);
	TLeagueApi(TLeagueApi<FPopInfo, FUnitInfo>&& Moved);

public:
	void Init(ILeagueBase<FPopInfo, FUnitInfo>* InBase, int PopCount, int PopSize);
	FDna& AccessDna(int PopIndex, int UnitIndex);
	const FDna& AccessDna(int PopIndex, int UnitIndex) const;
	FPopInfo& AccessPopInfo(int PopIndex);
	const FPopInfo& AccessPopInfo(int PopIndex) const;
	FUnitInfo& AccessUnitInfo(int PopIndex, int UnitIndex);
	const FUnitInfo& AccessUnitInfo(int PopIndex, int UnitIndex) const;
	void Clear();

	void IterateFor(int GenerationCount);
	void IterateTo(int FinalGeneration);
	int GetGeneration() const;
	int GetPopCount() const;
	int GetPopSize() const;

	void CreateThreadPool(int ThreadCount);
	void AssignThreadPool(int PoolId);
	void SetThreaded(bool bThreaded);

	void Internal_PlayMatch(TMatchData<FPopInfo, FUnitInfo>& Match);

private:
	void IterateSingle();

private:
	ILeagueBase<FPopInfo, FUnitInfo>* Base;
	int PopCount;
	int PopSize;
	int Generation;

	TArray<FDna> DataDna;
	TArray<FPopInfo> DataPops;
	TArray<FUnitInfo> DataUnits;

	bool bPlayThreaded = (LEAGUE_API_PREFERED_THREAD_FLAGS > 1);
	int AssignedThreadPool = -1;
	int PreferedThreadCount = LEAGUE_API_PREFERED_THREAD_FLAGS;
};

#include <Utils/LeaguePlay_Imp.h>
