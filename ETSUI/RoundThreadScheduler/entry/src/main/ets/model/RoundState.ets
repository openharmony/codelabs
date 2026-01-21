/**
 * RoundState
 *
 * Tracks execution progress of rounds.
 * Used mainly for UI and logging.
 */

export class RoundState {
  private currentRound: number = 0
  private totalRounds: number

  constructor(totalRounds: number) {
    this.totalRounds = totalRounds
  }

  updateRound(round: number): void {
    this.currentRound = round
  }

  getCurrentRound(): number {
    return this.currentRound
  }

  isFinished(): boolean {
    return this.currentRound >= this.totalRounds
  }

  reset(): void {
    this.currentRound = 0
  }
}
