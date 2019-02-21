import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { StartPageComponent } from './start-page/start-page.component';
import { AstraPageComponent } from './astra-page/astra-page.component';
import { ResultsPageComponent } from './results-page/results-page.component';

const routes: Routes = [
  {path: 'home' , component: StartPageComponent},
  {path: 'test' , component: AstraPageComponent},
  {path: 'results' , component: ResultsPageComponent},
  {path: '', redirectTo:'/home',pathMatch: 'full'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
