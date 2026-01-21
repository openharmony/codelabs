/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
