export interface DailyData {
  height: number;
  weight: number;
  age: number;
  bmi: number;
  bmr: number;
  recommendedWater: number;
  actualWater: number;
  exerciseDuration: number;
  actualCalories?: number;
  targetCalories?: number;
  targetExercise?: number;
}

export class UserStore {
  private static staticData: Record<string, DailyData> = {
    '2026-01-19': { height: 175, weight: 70.5, age: 25, bmi: 23.02, bmr: 1700, recommendedWater: 2000, actualWater: 2100, exerciseDuration: 30},
    '2026-01-18': { height: 175, weight: 71.0, age: 25, bmi: 23.18, bmr: 1700, recommendedWater: 2000, actualWater: 1800, exerciseDuration: 30}
  }

  static init() {
    let currentData: Record<string, DailyData> = AppStorage.Get('healthHistory') || {};
    Object.keys(this.staticData).forEach(key => {
      if (!currentData[key]) {
        currentData[key] = this.staticData[key];
      }
    });
    AppStorage.SetOrCreate('healthHistory', currentData);
  }

  static saveData(date: string, data: DailyData) {
    let currentData: Record<string, DailyData> = AppStorage.Get('healthHistory') || {};
    currentData[date] = data; // 覆盖或新增
    AppStorage.SetOrCreate('healthHistory', currentData);
  }

  static calculateWater(weight: number): number {
    return Math.round(weight * 35);
  }

  static updateTodayTarget(weight: number) {
    const today = new Date().toISOString().split('T')[0];
    const newTarget = this.calculateWater(weight);
    let history: Record<string, DailyData> = AppStorage.Get('healthHistory') || {};

    if (history[today]) {
      history[today].recommendedWater = newTarget;
    } else {
      history[today] = {
        height: 175, weight: weight, age: 25, bmi: 0, bmr: 0,
        recommendedWater: newTarget,
        actualWater: 0,
        exerciseDuration: 0
      };
    }

    AppStorage.SetOrCreate('healthHistory', history);
  }
}