import { AppComponent, BrandComponent, Connection, Settings } from '@ratiosolver/flick';
import { coco } from '@ratiosolver/coco';
import './style.css'
import { Offcanvas } from './offcanvas';

Settings.get_instance().load_settings({ ws_path: '/coco' });

const offcanvas_id = 'restart-offcanvas';

class CoCoApp extends AppComponent {

  constructor() {
    super();

    // Create and add brand element
    this.navbar.add_child(new BrandComponent('', 'logo.png', 96, 32, offcanvas_id));

    this.add_child(new Offcanvas(offcanvas_id));

    Connection.get_instance().connect();
  }

  override received_message(message: any): void { coco.CoCo.get_instance().update_coco(message); }

  override connection_error(_error: any): void { this.toast('Connection error. Please try again later.'); }
}

new CoCoApp();