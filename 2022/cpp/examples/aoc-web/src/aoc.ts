type WasmCallbacks = import("./WasmCallbacks").WasmCallbacks;
declare var Module: any;

const page = {
  body: document.body,
  filePicker: document.getElementById("filePicker") as HTMLInputElement,
  daySelector: document.getElementById("selectDay") as HTMLSelectElement,
  puzzleInput: document.getElementById("puzzleInput") as HTMLTextAreaElement,
  viewResults: [
    document.getElementById("viewResultPart1") as HTMLParagraphElement,
    document.getElementById("viewResultPart2") as HTMLParagraphElement,
  ],
  timingsOutputs: document.getElementById("timingsOutput") as HTMLParagraphElement
};

let lastInput = { day: 0, text: "" };
let endTimes: [number, number] = [performance.now(), performance.now()];

const aocWasmCallbacks: WasmCallbacks = {
  reiceivePuzzleSolution: (
    day: number,
    part: number,
    solution: string
  ): void => {
    // console.log([
    //   "result",
    //   day,
    //   part,
    //   solution,
    //   "viewResultPart" + part.toString(),
    // ]);
    const index = part - 1;
    if (day < 1 || day > 25 || part < 1 || part > page.viewResults.length) {
      if (part > 0 && part <= page.viewResults.length) {
        page.viewResults[index].innerText = "failed";
      }
      return;
    }
    endTimes[index] = performance.now();
    let view = page.viewResults[index];
    view.innerText = solution || "failed";
  },
};

Module["onRuntimeInitialized"] = (_) => {
  type SolveFunction = (day: number, input: string, inputLen: number) => number;
  type HasImplementationFunction = (day: number) => number;
  const solve_puzzle: SolveFunction = Module.cwrap("solve_puzzle", "number", [
    "number",
    "string",
    "number",
  ]);
  const has_implementation_for_day: HasImplementationFunction = Module.cwrap(
    "has_implementation_for_day",
    "number",
    ["number"]
  );

  let implementedDays: number[] = [];
  for (let day = 1; day < 26; day++) {
    if (has_implementation_for_day(day) === 1) {
      implementedDays.push(day);
    }
  }
  implementedDays.forEach((day) => {
    const newOption = document.createElement("option");
    newOption.value = day.toString();
    newOption.innerText = `Day ${day}`;
    page.daySelector.appendChild(newOption);
  });

  const onPuzzleInputChanged = () => {
    const day = parseInt(page.daySelector.value);
    const puzzleInput = page.puzzleInput.value;
    if ((day === lastInput.day) && (puzzleInput == lastInput.text)) {
      return;
    }
    lastInput.day = day;
    lastInput.text = puzzleInput;
    if (day < 1 || isNaN(day) || puzzleInput.length < 1) {
      page.viewResults.forEach((viewResult) => (viewResult.innerText = ""));
      // console.log("input error");
      return;
    }
    page.viewResults.forEach((result) => (result.innerText = "computing…"));
    const startTime = performance.now();
    solve_puzzle(day, puzzleInput, puzzleInput.length);
    const endTime = performance.now();
    const decimalPlaces = 5;
    page.timingsOutputs.innerHTML = `Part&nbsp;1: ${(endTimes[0] - startTime).toFixed(decimalPlaces)}&nbsp;ms | Part&nbsp;2: ${(endTimes[1] - endTimes[0]).toFixed(decimalPlaces)}&nbsp;ms | Total ${(endTime - startTime).toFixed(decimalPlaces)}&nbsp;ms`;
  };

  const loadPuzzleInputFile = (file: File) => {
    if (!file.name.endsWith(".txt")) {
      return;
    }
    const numStrFromFileName = file.name.match(/\d+/);
    if (numStrFromFileName?.length > 0) {
      const numFromFileName = parseInt(numStrFromFileName[0]);
      if (
        !isNaN(numFromFileName) &&
        implementedDays.indexOf(numFromFileName) >= 0
      ) {
        page.daySelector.value = numFromFileName.toString();
      }
    }
    let reader = new FileReader();
    // console.log("file changed");
    reader.addEventListener("load", (event) => {
      if (typeof event.target.result === "string") {
        // console.log("file read");
        page.puzzleInput.value = event.target.result;
        onPuzzleInputChanged();
      } else {
        // console.log("error reading file");
      }
    });
    reader.readAsText(file);
  };

  page.filePicker.addEventListener("change", (event) => {
    const file = (event.target as HTMLInputElement).files[0];
    loadPuzzleInputFile(file);
  });

  page.puzzleInput.addEventListener("drop", (event) => {
    event.preventDefault();
    page.puzzleInput.classList.remove("dragover");
    const file =
      event.dataTransfer?.items?.length === 1
        ? event.dataTransfer.items[0].getAsFile()
        : event.dataTransfer?.files?.length === 1
        ? event.dataTransfer?.files[0]
        : undefined;
    if (file != undefined) {
      loadPuzzleInputFile(file);
    }
  });

  page.body.addEventListener("dragover", (event) => {
    console.log("dragover");
    page.puzzleInput.classList.add("dragover");
  });
  page.body.addEventListener("drageave", (event) => {
    console.log("drageave");
    page.puzzleInput.classList.add("dragover");
  });
  page.body.addEventListener("dragenter", (event) => {
    console.log("dragenter");
    page.puzzleInput.classList.add("dragover");
  });
  page.body.addEventListener("dragleave", (event) => {
    console.log("drageave");
    page.puzzleInput.classList.remove("dragover");
  });

  page.daySelector.addEventListener("change", (_) => onPuzzleInputChanged());

  const onPuzzleAreaEvent = (event) => {
    onPuzzleInputChanged();
  };

  page.puzzleInput.addEventListener("keyup", (_) => {
    onPuzzleInputChanged();
  });
  page.puzzleInput.addEventListener("change", (_) => {
    onPuzzleInputChanged();
  });
  page.puzzleInput.addEventListener("paste", (_) => {
    onPuzzleInputChanged();
  });

};
