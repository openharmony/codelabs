import { ContactModel } from '../viewmodel/ContactModel';
export interface ContactGroup {
  index: ResourceStr
  contacts: ContactModel[]
}