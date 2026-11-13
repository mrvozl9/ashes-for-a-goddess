// AI-assisted: Claude Code, 13.11.2026, word-by-word reply: reads the "reply" field out of the model's JSON while it is still being written (Phase 7). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"

struct FAshesValidationContext;

/**
 *  Helpers for showing a reply while the model writes it. The model writes one JSON object
 *  ({"reply": "...", "emotion": ..., ...}); these read what is already there of the "reply" string.
 *  Nothing here is validated: the finished reply still goes through UResponseValidator.
 */
namespace AshesReplyStream
{
	/**
	 *  The "reply" text written so far: decodes \" \\ \n \t \uXXXX, stops at the closing quote or at the end of the
	 *  text (a half-written escape at the end is left out), removes * and _ markup like the validator does.
	 *  Empty while the "reply" key or its opening quote has not arrived yet.
	 */
	ASHES_API FString ExtractReply(const FString& PartialJson);

	/** True if the shown text already contains a locked spoiler keyword or an unsafe word (stop showing it) */
	ASHES_API bool IsBlocked(const FString& ReplySoFar, const FAshesValidationContext& Context);

	/**
	 *  The part of ReplySoFar that may be on screen now: whole words only (up to the last space), and without the
	 *  last word(s) when they could be the start of a locked spoiler keyword or an unsafe phrase
	 *  ("Yes, Vashir" waits while "Vashir hired" is locked). Empty when nothing is safe to show yet.
	 */
	ASHES_API FString SafeToShow(const FString& ReplySoFar, const FAshesValidationContext& Context);
}
