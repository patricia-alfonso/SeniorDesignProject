import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'App',
  templateUrl: './app.component.html',
})
export class AppComponent implements OnInit {
  public readonly name = 'Astra Body Tracker';

  ngOnInit(): void {
    console.log('component initialized');
  }
}

@NgModule({
  imports: [BrowserModule],
  declarations: [AppComponent],
  bootstrap: [AppComponent]
})
export class AppModule { }