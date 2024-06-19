import AbstractView from "./AbstractView.js";

const DeleteView = (type) => class extends AbstractView {
    constructor(params) {
        super(params);
    }

    deleteEntityHTML = () => {
        return `
            <h1>Delete ${type} with id ${this.params.uuid}</h1>
            <p>Are you sure you wish to delete ${type} with id ${this.params.uuid}?</p>
            <form action="/api/${type}s/delete" method="post">
                <input type="hidden" name="uuid" value="${this.params.uuid}"/>
                <input type="submit" value="Yes"/>
                <a href="/${type}s">No</a>
            </form>
        `
    };

    deleteFileHTML = () => {
        const searchParams = new URLSearchParams(window.location.search);
        const path = searchParams.get("file");
        if (!path) {
            return "Missing file parameter";
        }

        return `
            <h1>Delete file ${path}</h1>
            <p>Are you sure you wish to delete ${path}? This action can not be undone!</p>
            <form action="/api/filesystem/delete" method="post">
                <input type="hidden" name="path" value="${path}"/>
                <input type="submit" value="Yes"/>
                <a href="/filesystem">No</a>
            </form>
        `
    };

    async getHtml() {
        switch (type) {
            case "file": return this.deleteFileHTML();
            default: return this.deleteEntityHTML();
        }
    }
}

export default DeleteView;
