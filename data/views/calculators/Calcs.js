import AbstractView from "../AbstractView.js";
import { getCalculatorTypes } from "./Helpers.js";

export default class Calculators extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle("Calculators");
    }

    async getHtml() {
        const calculators = await fetch("/api/calculators")
            .then(response => response.json())
            .then(json => json ? json : {});
        const calculatorTypes = await getCalculatorTypes();

        return `
            <h1>Calculators</h1>
            <table>
            <thead>
                <tr>
                    <td>Name</td>
                    <td>Type</td>
                    <td>Valid</td>
                    <td></td>
                </tr>
            </thead>
            <tbody>
            ${
            Object.entries(calculators)
                .map(([uuid, info]) => `
                <tr>
                    <td>${info.name}</td>
                    <td>${calculatorTypes[info.type]}</td>
                    <td>${info.valid}</td>
                    <td>
                        <ul>
                            <li><a href="/calculators/${uuid}" data-link="">Details</a></li>
                            <li><a href="/calculators/${uuid}/edit" data-link="">Edit</a></li>
                            <li><a href="/calculators/${uuid}/delete" data-link="">Delete</a></li>
                        </ul>
                    </td>
                </tr>
                `)
                .join("\n")
            }
            </tbody>
            </table>


            <h2>Add new calculator</h2>
            <ul>
            ${
            Object.entries(calculatorTypes)
            .map(([k, v]) => `<li><a href="/calculators/add/${k}" data-link="">${v}</a></li>`)
            .join("\n")
            }
            </ul>
        `;
    }
}
