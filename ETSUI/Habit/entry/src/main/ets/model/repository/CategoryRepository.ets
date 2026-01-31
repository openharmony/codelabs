import RdbHelper from '../database/RdbHelper';
import Category from '../../common/bean/Category';
import { CommonConstants } from '../../common/constants/CommonConstants';
import relationalStore from '@ohos.data.relationalStore';

class CategoryRepository {

  async queryAll(): Promise<Category[]> {
    const store = RdbHelper.getRdbStore();
    if (!store) return [];

    let predicates = new relationalStore.RdbPredicates(CommonConstants.CATEGORY_TABLE_NAME);
    const resultSet = await store.query(predicates);

    let list: Category[] = [];
    while (resultSet.goToNextRow()) {
      let cat = new Category(
        resultSet.getString(resultSet.getColumnIndex('name')),
        resultSet.getString(resultSet.getColumnIndex('color'))
      );
      cat.id = resultSet.getLong(resultSet.getColumnIndex('id'));
      list.push(cat);
    }
    resultSet.close();
    return list;
  }
  insert(name: string, color: string): Promise<number> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB not initialized');

    const valueBucket: relationalStore.ValuesBucket = {
      name: name,
      color: color
    };

    return store.insert(CommonConstants.CATEGORY_TABLE_NAME, valueBucket);
  }

  async cleanAll(): Promise<void> {
    const store = RdbHelper.getRdbStore();
    if (!store) return Promise.reject('DB not initialized');

    await store.executeSql('DELETE FROM category WHERE id > 1');
  }
}

export default new CategoryRepository();