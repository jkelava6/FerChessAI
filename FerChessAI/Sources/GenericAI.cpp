


#include <StdH.h>
#include <GenericAI.h>

bool FEvaluatedMove::operator== (const FEvaluatedMove& Other) const
{
	return RankFrom == Other.RankFrom && FileFrom == Other.FileFrom
		&& RankTo == Other.RankTo && FileTo == Other.FileTo;
}

bool FEvaluatedMove::operator!= (const FEvaluatedMove& Other) const
{
	return !(*this == Other);
}

