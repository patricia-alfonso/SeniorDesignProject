import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { AstraPageComponent } from './astra-page.component';

describe('AstraPageComponent', () => {
  let component: AstraPageComponent;
  let fixture: ComponentFixture<AstraPageComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ AstraPageComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(AstraPageComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
