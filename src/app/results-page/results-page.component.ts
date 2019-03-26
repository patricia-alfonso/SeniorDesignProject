import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-results-page',
  templateUrl: './results-page.component.html',
  styleUrls: ['./results-page.component.css']
})
export class ResultsPageComponent implements OnInit {
  res_color(x) {
    let elt = document.getElementById("resultText");
    if (x < 70) {
      elt.className = "p-3 mb-2 bg-danger text-white";
    } else if (x < 90) {
      elt.className = "p-3 mb-2 bg-warning text-white";
    } else {
      elt.className = "p-3 mb-2 bg-success text-white";
    }
  }

  constructor() {

  }

  ngOnInit() {
    var calculated_result = 75;
    this.res_color(calculated_result);
  }

}
