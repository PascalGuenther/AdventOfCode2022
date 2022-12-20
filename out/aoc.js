const page = {
    body: document.body,
    filePicker: document.getElementById("filePicker"),
    daySelector: document.getElementById("selectDay"),
    puzzleInput: document.getElementById("puzzleInput"),
    viewResults: [
        document.getElementById("viewResultPart1"),
        document.getElementById("viewResultPart2"),
    ],
    timingsOutputs: document.getElementById("timingsOutput")
};
let lastInput = { day: 0, text: "" };
let endTimes = [performance.now(), performance.now()];
const aocWasmCallbacks = {
    reiceivePuzzleSolution: (day, part, solution) => {
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
    const solve_puzzle = Module.cwrap("solve_puzzle", "number", [
        "number",
        "string",
        "number",
    ]);
    const has_implementation_for_day = Module.cwrap("has_implementation_for_day", "number", ["number"]);
    let implementedDays = [];
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
    const loadPuzzleInputFile = (file) => {
        if (!file.name.endsWith(".txt")) {
            return;
        }
        const numStrFromFileName = file.name.match(/\d+/);
        if ((numStrFromFileName === null || numStrFromFileName === void 0 ? void 0 : numStrFromFileName.length) > 0) {
            const numFromFileName = parseInt(numStrFromFileName[0]);
            if (!isNaN(numFromFileName) &&
                implementedDays.indexOf(numFromFileName) >= 0) {
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
            }
            else {
                // console.log("error reading file");
            }
        });
        reader.readAsText(file);
    };
    page.filePicker.addEventListener("change", (event) => {
        const file = event.target.files[0];
        loadPuzzleInputFile(file);
    });
    page.puzzleInput.addEventListener("drop", (event) => {
        var _a, _b, _c, _d, _e;
        event.preventDefault();
        page.puzzleInput.classList.remove("dragover");
        const file = ((_b = (_a = event.dataTransfer) === null || _a === void 0 ? void 0 : _a.items) === null || _b === void 0 ? void 0 : _b.length) === 1
            ? event.dataTransfer.items[0].getAsFile()
            : ((_d = (_c = event.dataTransfer) === null || _c === void 0 ? void 0 : _c.files) === null || _d === void 0 ? void 0 : _d.length) === 1
                ? (_e = event.dataTransfer) === null || _e === void 0 ? void 0 : _e.files[0]
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
