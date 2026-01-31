/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import dataPreferences from '@ohos.data.preferences';

let context = getContext(this);
let preference: dataPreferences.Preferences;
let preferenceTemp: dataPreferences.Preferences;

/**
 * Health consult favorite preference model.
 */
class HealthConsultFavoriteModel {
  private favoriteData: Set<number> | null = null;

  /**
   * Read the specified Preferences persistence file and load the data into the Preferences instance.
   */
  async getPreferencesFromStorage() {
    try {
      preference = await dataPreferences.getPreferences(context, 'HealthPreferences');
    } catch (err) {
      console.error(`Failed to get preferences, Cause: ${err}`);
    }
  }

  /**
   * Deletes the specified Preferences persistence file from memory and removes the Preferences instance.
   */
  async deletePreferences() {
    try {
      await dataPreferences.deletePreferences(context, 'HealthPreferences');
    } catch (err) {
      console.error(`Failed to delete preferences, Cause: ${err}`);
    }
    ;
    preference = preferenceTemp;
  }

  /**
   * Get or initialize the get all favorite data (in set form).
   */
  async getFavoriteSetData(): Promise<Set<number>> {
    if (!preference) {
      await this.getPreferencesFromStorage();
    }
    if (this.favoriteData == null) {
      try {
        this.favoriteData = new Set<number>(await preference.get('FavoriteConsultIds', []) as Array<number>);
      } catch (err) {
        console.error(`Failed to init value, Cause: ${err}`);
      }
    }
    return this.favoriteData!
  }

  /**
   * Set health consult favorite state.
   */
  async setFavorite(id: number, isFavorite: boolean) {
    let favoriteData1 = await this.getFavoriteSetData();
    try {
      if (isFavorite) {
        favoriteData1.add(id);
      } else {
        favoriteData1.delete(id);
      }
      await preference.put('FavoriteConsultIds', Array.from(favoriteData1));
    } catch (err) {
      console.error(`Failed to put value, Cause: ${err}`);
    }
    // Store the Preference instance in the preference persistence file
    await preference.flush();
  }

  /**
   * Get health consult favorite state.
   */
  async getFavorite(id: number) {
    let favoriteData1 = await this.getFavoriteSetData();
    return favoriteData1.has(id);
  }
}

export default new HealthConsultFavoriteModel();
