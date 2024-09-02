function complexComputation() {
    let arr = [];
    for (let i = 0; i < 1e6; i++) {
        arr.push(Math.sqrt(i) * Math.sin(i));
    }

    let sum = 0;
    for (let i = 0; i < arr.length; i++) {
        sum += arr[i];
    }

    return sum;
}

function main() {
    let startTime = new Date().getTime();
    let result = complexComputation();
    let endTime = new Date().getTime();
    let duration = endTime - startTime;

    console.log("Complex Computation took " + duration + " milliseconds");
    console.log("Result:", result);
}

// Call the main function explicitly
main();
